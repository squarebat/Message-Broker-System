//
// Created by mando on 12/01/21.
//

#define CROW_ENABLE_SSL

#include <iostream>
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>
#include <jwt-cpp/jwt.h>
#include <curl/curl.h>
#include "SystemContext.h"

namespace po = boost::program_options;

SystemContext & SystemContext::GenerateContext(int argc, char** argv) {
    static SystemContext systemContext;

    po::options_description description("usage: eventflow [options]");
    description.add_options()
            ("help", "shows this help message")
            ("config", po::value<std::string>(),
             "specify location of configuration file (Defaults to config.yaml)")
            ("authinfo", po::value<std::string>(),
             "specify location of client authentication info file (Defaults to clientinfo)")
            ("port", po::value<uint16_t>(),
             "specify port number for REST API (Defaults to 18080)")
            ("threads", po::value<uint16_t>(),
             "specify number of threads for REST API (Defaults to the number of threads supported by "
             "the hardware)")
            ("crt", po::value<std::string>(),
             "specify SSL Public Key file location (Defaults to host.crt")
            ("key", po::value<std::string>(),
             "specify SSL Private Key file location (Defaults to host.key")
            ;

    po::variables_map variablesMap;
    po::store(po::parse_command_line(argc, argv, description), variablesMap);
    po::notify(variablesMap);

    if (variablesMap.count("help")) {
        std::cout << description << std::endl;
        exit(1);
    }

    std::string configFile;
    if (variablesMap.count("config")) {
        configFile.assign(variablesMap["config"].as<std::string>());
    } else {
        configFile.assign("config.yaml");
    }

    std::string clientinfoFile;
    if (variablesMap.count("authinfo")) {
        clientinfoFile.assign(variablesMap["authinfo"].as<std::string>());
    } else {
        clientinfoFile.assign("clientinfo");
    }

    const YAML::Node config = YAML::LoadFile(configFile);

    // Setting Port number for REST API
    if (variablesMap.count("port")) {
        systemContext.port_no = variablesMap["port"].as<uint16_t>();
    } else if (config["port"]){
        systemContext.port_no = config["port"].as<uint16_t>();
    } else {
        systemContext.port_no = 18080;
    }

    // Setting Number of threads for REST API
    if (variablesMap.count("threads")) {
        systemContext.num_api_threads = variablesMap["threads"].as<uint16_t>();
    } else if (config["threads"]){
        systemContext.num_api_threads = config["threads"].as<uint16_t>();
    } else {
        systemContext.num_api_threads = 0;
    }

    // Setting SSL Public Key file location
    if (variablesMap.count("crt")) {
        systemContext.crt_file_path.assign(variablesMap["crt"].as<std::string>());
    } else if (config["crt"]){
        systemContext.crt_file_path.assign(config["crt"].as<std::string>());
    } else {
        systemContext.crt_file_path.assign("host.crt");
    }

    // Setting SSL Private Key file location
    if (variablesMap.count("key")) {
        systemContext.key_file_path.assign(variablesMap["key"].as<std::string>());
    } else if (config["key"]){
        systemContext.key_file_path.assign(config["key"].as<std::string>());
    } else {
        systemContext.key_file_path.assign("host.key");
    }

    // Setting JWT Secret Key
    if (config["jwt_secret"]) {
        systemContext.jwt_secret_key.assign(config["jwt_secret"].as<std::string>());
    } else {
        std::cerr << "jwt_secret required. Please provide it in the config file.\n";
        exit(1);
    }

    // Setting JWT Token Validity
    if (config["jwt_validity"]) {
        systemContext.token_validity = config["jwt_validity"].as<long>();
    } else {
        systemContext.token_validity = 24;
    }

    const YAML::Node& topics = config["topics"];
    std::vector<std::string> topic_names;
    std::vector<std::string> client_names;

    topic_names.reserve(topics.size());

    for (const auto & topic : topics) {
        std::string topic_name = topic["name"].as<std::string>();
        topic_names.emplace_back(topic_name);
        systemContext.topics[topic_name] = Topic(topic_name);
        for (int j = 0; j < topic["publishers"].size(); j++) {
            client_names.emplace_back(topic["publishers"][j].as<std::string>());
        }
        for (int j = 0; j < topic["subscribers"].size(); j++) {
            client_names.emplace_back(topic["subscribers"][j].as<std::string>());
        }
    }

    auto _ = std::unique(topic_names.begin(), topic_names.end());
    _ = std::unique(client_names.begin(), client_names.end());

    systemContext.accessList = std::make_unique<AccessList>(topic_names, client_names);

    for (const auto & topic : topics) {
        for (int j = 0; j < topic["publishers"].size(); j++) {
            client_names.emplace_back();
            systemContext.accessList->addAsPublisherOf(topic["publishers"][j].as<std::string>(),
                                                       topic["name"].as<std::string>());
        }
        for (int j = 0; j < topic["subscribers"].size(); j++) {
            client_names.emplace_back();
            systemContext.accessList->addAsSubscriberOf(topic["subscribers"][j].as<std::string>(),
                                                        topic["name"].as<std::string>());
        }
    }

    systemContext.authenticationData = new AuthenticationData(clientinfoFile);
    systemContext.authenticationData->LoadData();

    return systemContext;
}

void SystemContext::StartAPI() {
    /*
     * request-data:
     *      "name" - Client name,
     *      "password" - Client password,
     *      "notify_on" - Notification port no
     * response-data:
     *      if authenticated: (200)
     *          "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      else: (401)
     *          "error" - States the error
     */
    CROW_ROUTE(app, "/auth")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        auto body = crow::json::load(req.body);
                        std::string name(body["name"].s());
                        std::string password(body["password"].s());
                        std::string notify_on(body["notify_on"].s());
                        crow::json::wvalue response;
                        if (authenticationData->AuthenticateClient(name, password)) {
                            AddClient(name, req.ip_address, notify_on);
                            auto token = jwt::create()
                                    .set_issuer("eventflow")
                                    .set_issued_at(std::chrono::system_clock::now())
                                    .set_payload_claim("name", jwt::claim(name))
                                    .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{token_validity})
                                    .sign(jwt::algorithm::hs256{jwt_secret_key});
                            response["token"] = token;
                            for (auto& topic: topics) {
                                if (accessList->isSubscriberOf(name, topic.first)) {
                                    topic.second.increment_num_active_clients();
                                }
                            }
                            return crow::response(200, response);
                        } else {
                            response["error"] = "Authorization data invalid.";
                            return crow::response(401, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      "topic" - Topic to which the event is to be published.
     *      "event" - Event / message that is to be published.
     * response-data:
     *      if authorized:
     *          200 - Event published successfully.
     *      else:
     *          404 - Topic does not exist / Invalid Request.
     *          401 - Unauthorised action (Invalid token / Not a publisher of topic).
     */
    CROW_ROUTE(app, "/publish")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto name = claims["name"];
                            std::string topic_name(body["topic"].s());
                            std::string event(body["event"].s());
                            if (!accessList->isPublisherOf(name.as_string(), topic_name)) {
                                response["error"] = "Unauthorized action";
                                return crow::response(401, response);
                            }
                            if (topic_name.empty() || event.empty()) {
                                response["error"] = "Invalid Request";
                                return crow::response(404, response);
                            }
                            try {
                                Topic& topic = topics.at(topic_name);
                                topic.pub_event(event);
                                std::thread thread([this] (const std::string& topic_name) {
                                    CURL *curl;
                                    for (auto& client: clients) {
                                        if (accessList->isSubscriberOf(client.first, topic_name)) {
                                            curl = curl_easy_init();
                                            if (curl) {
                                                std::string url = std::string("http://") + client.second.ip_address()
                                                                  + std::string(":") + client.second.notif_port_no() + std::string("/");
                                                std::string post_fields = "topic=" + topic_name;
                                                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                                                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
                                                curl_easy_perform(curl);
                                            }
                                            curl_easy_cleanup(curl);
                                        }
                                    }
                                    curl_global_cleanup();
                                }, topic_name);
                                response["status"] = "Event published successfully";
                                return crow::response(200, response);
                            } catch (const std::out_of_range& ex) {
                                response["error"] = "Topic does not exist";
                                return crow::response(404, response);
                            }
                        } catch (const std::exception& ex) {
                            response["error"] = "Invalid client";
                            return crow::response(401, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      "topic" - Topic from which the event is to be fetched.
     * response-data:
     *      if authorized (200):
     *          "event" - Event.
     *      else:
     *          404 - Topic does not exist / Invalid Request.
     *          401 - Unauthorised action (Invalid token / Not a subscriber of topic).
     */
    CROW_ROUTE(app, "/")
            .methods("GET"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto name = claims["name"];
                            std::string topic_name(body["topic"].s());
                            Client& client = clients[name.as_string()];
                            if (!(accessList->isSubscriberOf(client.name(), topic_name))) {
                                response["error"] = "Unauthorized action";
                                return crow::response(401, response);
                            }
                            if (topic_name.empty()) {
                                response["error"] = "Invalid Request";
                                return crow::response(404, response);
                            }
                            try {
                                Topic& topic = topics.at(topic_name);
                                response["event"] = topic.get_event_for(client).message;
                                return crow::response(200, response);
                            } catch (const std::out_of_range& ex) {
                                response["error"] = "Topic does not exist";
                                return crow::response(404, response);
                            }
                        } catch (const std::exception& ex) {
                            response["error"] = "Invalid client";
                            return crow::response(401, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     * response-data:
     *      if authorized:
     *          200 - Disconnected successfully.
     *      else:
     *          404 - Invalid Request.
     *          401 - Invalid Client.
     */
    CROW_ROUTE(app, "/disconnect")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto name = claims["name"];
                            if (clients.find(name.as_string()) != clients.end()) {
                                for (auto& topic: topics) {
                                    if (accessList->isSubscriberOf(name.as_string(), topic.first)) {
                                        topic.second.decrement_num_active_clients();
                                    }
                                }
                                response["status"] = "Disconnected successfully";
                                return crow::response(200, response);
                            }
                            response["error"] = "Invalid Request";
                            return crow::response(404, response);
                        } catch (std::exception& ex) {
                            response["error"] = "Invalid client";
                            return crow::response(401, response);
                        }
                    });

    app.port(port_no).multithreaded(num_api_threads).run();
//    app.port(port_no).multithreaded(num_api_threads).ssl_file(crt_file_path, key_file_path).run();
}

void SystemContext::AddClient(const std::string& name, const std::string& ip_address, const std::string& notif_port_no) {
    mutex_lock.lock();
    clients.insert({name, Client(name, ip_address, notif_port_no)});
    mutex_lock.unlock();
}
