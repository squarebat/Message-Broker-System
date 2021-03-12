//
// Created by mando on 12/01/21.
//

#define CROW_ENABLE_SSL

#include <iostream>
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>
#include <jwt-cpp/jwt.h>
#include <curl/curl.h>
#include <SystemContext.h>
#include <publish_to_topic_sink.h>
#include <string>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>

namespace po = boost::program_options;
const string SystemContext::logger_name = "status_log";
bool SystemContext::instantiated = false;

// To prevent libcurl output
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    return size * nmemb;
}

std::string get_current_time() {
    auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&current_time), "%Y-%m-%d %X");
    return ss.str();
}

SystemContext& SystemContext::GenerateContext(int argc, char** argv) {
    static SystemContext systemContext;
    if (instantiated) {
        return systemContext;
    }

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
             "specify SSL Private Key file location (Defaults to host.key");

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

    // Setting Port Number For REST API
    if (variablesMap.count("port")) {
        systemContext.port_no = variablesMap["port"].as<uint16_t>();
    } else if (config["port"]) {
        systemContext.port_no = config["port"].as<uint16_t>();
    } else {
        systemContext.port_no = 18080;
    }

    // Setting Number Of Threads For REST API
    if (variablesMap.count("threads")) {
        systemContext.num_api_threads = variablesMap["threads"].as<uint16_t>();
    } else if (config["threads"]) {
        systemContext.num_api_threads = config["threads"].as<uint16_t>();
    } else {
        systemContext.num_api_threads = 0;
    }

    // Setting SSL Public Key File Location
    if (variablesMap.count("crt")) {
        systemContext.crt_file_path.assign(variablesMap["crt"].as<std::string>());
    } else if (config["crt"]) {
        systemContext.crt_file_path.assign(config["crt"].as<std::string>());
    } else {
        systemContext.crt_file_path.assign("host.crt");
    }

    // Setting SSL Private Key File Location
    if (variablesMap.count("key")) {
        systemContext.key_file_path.assign(variablesMap["key"].as<std::string>());
    } else if (config["key"]) {
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

    // Setting Status Log Topic Name
    std::string status_log_topic_name;
    if (config["status_log_topic"]) {
        status_log_topic_name = config["status_log_topic"].as<std::string>();
    } else {
        status_log_topic_name = "";
    }

    // Setting Logger Thread Count
    long logger_thread_count;
    if (config["logger_thread_count"]) {
        logger_thread_count = config["logger_thread_count"].as<long>();
    } else {
        logger_thread_count = 1;
    }

    // Setting Logger Queue Size
    long logger_queue_size;
    if (config["logger_queue_size"]) {
        logger_queue_size = config["logger_queue_size"].as<long>();
    } else {
        logger_queue_size = 8192;
    }

    const YAML::Node &topics = config["topics"];
    std::vector<std::string> topic_names;
    std::vector<std::string> client_names;

    topic_names.reserve(topics.size());

    for (const auto &topic : topics) {
        topic_names.emplace_back(topic["name"].as<std::string>());
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

    for (const auto &topic : topics) {
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

//    topic_names.erase(std::remove(topic_names.begin(), topic_names.end(), status_log_topic_name));

    for (auto &topic_name : topic_names) {
        systemContext.topics[topic_name] = Topic(topic_name);
    }

    if (!status_log_topic_name.empty()) {
        spdlog::init_thread_pool(logger_queue_size, logger_thread_count);
        auto log_publish_sink = std::make_shared<spdlog::sinks::publish_to_topic_sink_mt>(status_log_topic_name);
        auto logger = std::make_shared<spdlog::async_logger>(logger_name,
                                                             log_publish_sink, spdlog::thread_pool());
        spdlog::register_logger(logger);
    }

    systemContext.authenticationData = new AuthenticationData(clientinfoFile);
    systemContext.authenticationData->LoadData();

    instantiated = true;
    return systemContext;
}

void SystemContext::StartAPI() {
    /*
     * request-data:
     *      "name" - Client name.
     *      "password" - Client password.
     *      "notify_on" - Notification port no.
     * response-data:
     *      if authenticated: (200)
     *          "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      else: (400: Bad Request - Authorization Data Invalid,
     *          400: Bad Request - Missing Required Information,
     *          500: Internal Server Error)
     *          "error" - States the error.
     */
    CROW_ROUTE(app, "/auth")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        crow::json::wvalue log_data;
                        auto logger = spdlog::get(logger_name);
                        if (logger != nullptr) {
                            log_data["time"] = get_current_time();
                            log_data["request"] = "AUTHENTICATION";
                        }
                        try {
                            auto body = crow::json::load(req.body);
                            std::string name(body["name"].s());
                            std::string password(body["password"].s());
                            std::string notify_on(body["notify_on"].s());
                            if (authenticationData->AuthenticateClient(name, password)) {
                                if (logger != nullptr) {
                                    log_data["client"] = name;
                                }
                                req.get_header_value("Date");
                                AddClient(name, req.ip_address, notify_on);
                                auto token = jwt::create()
                                        .set_issuer("eventflow")
                                        .set_issued_at(std::chrono::system_clock::now())
                                        .set_payload_claim("name", jwt::claim(name))
                                        .set_expires_at(
                                                std::chrono::system_clock::now() + std::chrono::hours{token_validity})
                                        .sign(jwt::algorithm::hs256{jwt_secret_key});
                                response["token"] = token;
                                for (auto& topic: topics) {
                                    if (accessList->isSubscriberOf(name, topic.first)) {
                                        topic.second.increment_num_active_clients();
                                    }
                                }
                                if (logger != nullptr) {
                                    log_data["message"] = "Client Authenticated";
                                    logger->log(spdlog::level::info, crow::json::dump(log_data));
                                }
                                return crow::response(200, response);
                            } else {
                                response["error"] = "Bad Request - Authorization Data Invalid";
                                if (logger != nullptr) {
                                    log_data["message"] = "Authorization Data Invalid";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(400, response);
                            }
                        } catch (const std::runtime_error& ex) {
                            response["error"] = "Bad Request - Missing Required Information";
                            if (logger != nullptr) {
                                log_data["message"] = "Missing Required Information";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(400, response);
                        } catch (std::exception& ex) {
                            response["error"] = "Internal Server Error";
                            if (logger != nullptr) {
                                log_data["message"] = "Internal Server Error";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(500, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      "topic" - Topic to which the event is to be published.
     *      "event" - Event / message that is to be published.
     * response-data:
     *      if authorized: (201: Created - Event Published Successfully)
     *          "status" - States the status.
     *      else: (403: Forbidden - Client Not A Publisher Of Topic,
     *          400: Bad Request - Empty Topic / Event Information,
     *          404: Not Found - Topic Does Not Exist,
     *          409: Conflict - No Such Active Client,
     *          401: Unauthenticated - Invalid Token Supplied,
     *          400: Bad Request - Missing Required Information,
     *          500: Internal Server Error)
     *          "error" - States the error.
     */
    CROW_ROUTE(app, "/publish")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        crow::json::wvalue log_data;
                        auto logger = spdlog::get(logger_name);
                        if (logger != nullptr) {
                            log_data["time"] = get_current_time();
                            log_data["request"] = "PUBLISH";
                        }
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto _name = claims["name"];
                            auto name = _name.as_string();
                            std::string topic_name(body["topic"].s());
                            std::string event(body["event"].s());
                            Client& client = clients.at(name);
                            if (logger != nullptr) {
                                log_data["client"] = client.name();
                            }
                            if (!accessList->isPublisherOf(client.name(), topic_name)) {
                                response["error"] = "Forbidden - Client Not A Publisher Of Topic";
                                if (logger != nullptr) {
                                    log_data["message"] = "Client Not A Publisher Of Topic";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(403, response);
                            }
                            if (topic_name.empty() || event.empty()) {
                                response["error"] = "Bad Request - Empty Topic / Event Information";
                                if (logger != nullptr) {
                                    log_data["message"] = "Empty Topic / Event Information";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(400, response);
                            }
                            try {
                                Topic& topic = topics.at(topic_name);
                                if (logger != nullptr) {
                                    log_data["topic"] = topic.name;
                                }
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
                                                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                                                curl_easy_perform(curl);
                                            }
                                            curl_easy_cleanup(curl);
                                        }
                                    }
                                    curl_global_cleanup();
                                }, topic_name);
                                thread.detach();
                                response["status"] = "Created - Event Published Successfully";
                                if (logger != nullptr) {
                                    log_data["message"] = "Event Published Successfully";
                                    logger->log(spdlog::level::info, crow::json::dump(log_data));
                                }
                                return crow::response(201, response);
                            } catch (const std::out_of_range& ex) {
                                response["error"] = "Not Found - Topic Does Not Exist";
                                if (logger != nullptr) {
                                    log_data["message"] = "Topic Does Not Exist";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(404, response);
                            }
                        } catch (const std::out_of_range& ex) {
                            response["error"] = "Conflict - No Such Active Client";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(409, response);
                        } catch (const std::invalid_argument& ex) {
                            response["error"] = "Unauthenticated - Invalid Token Supplied";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(401, response);
                        } catch (const std::runtime_error& ex) {
                            response["error"] = "Bad Request - Missing Required Information";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(400, response);
                        } catch (std::exception& ex) {
                            response["error"] = "Internal Server Error";
                            if (logger != nullptr) {
                                log_data["message"] = "Internal Server Error";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(500, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     *      "topic" - Topic from which the event is to be fetched.
     * response-data:
     *      if authorized:
     *          if no events: (204: No Content - No New Events)
     *              "status" - States the status.
     *          else: (200)
     *              "event" - Event.
     *      else: (403: Forbidden - Client Not A Subscriber Of Topic,
     *          400: Bad Request - Empty Topic Information,
     *          404: Not Found - Topic Does Not Exist,
     *          409: Conflict - No Such Active Client,
     *          401: Unauthenticated - Invalid Token Supplied,
     *          400: Bad Request - Missing Required Information,
     *          500: Internal Server Error)
     *          "error" - States the error.
     */
    CROW_ROUTE(app, "/")
            .methods("GET"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        crow::json::wvalue log_data;
                        auto logger = spdlog::get(logger_name);
                        if (logger != nullptr) {
                            log_data["time"] = get_current_time();
                            log_data["request"] = "FETCH";
                        }
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto _name = claims["name"];
                            std::string name = _name.as_string();
                            std::string topic_name(body["topic"].s());
                            Client& client = clients.at(name);
                            if (logger != nullptr) {
                                log_data["client"] = client.name();
                            }
                            if (!(accessList->isSubscriberOf(client.name(), topic_name))) {
                                response["error"] = "Forbidden - Client Not A Subscriber Of Topic";
                                if (logger != nullptr) {
                                    log_data["message"] = "Client Not A Subscriber Of Topic";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(403, response);
                            }
                            if (topic_name.empty()) {
                                response["error"] = "Bad Request - Empty Topic Information";
                                if (logger != nullptr) {
                                    log_data["message"] = "Empty Topic Information";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(400, response);
                            }
                            try {
                                Topic& topic = topics.at(topic_name);
                                if (logger != nullptr) {
                                    log_data["topic"] = topic.name;
                                }
                                try {
                                    response["event"] = topic.get_event_for(client).message;
                                    if (logger != nullptr) {
                                        log_data["message"] = "Message Fetched Successfully";
                                        logger->log(spdlog::level::info, crow::json::dump(log_data));
                                    }
                                    return crow::response(200, response);
                                } catch (const std::out_of_range& ex) {
                                    response["status"] = "No Content - No New Events";
                                    if (logger != nullptr) {
                                        log_data["message"] = "No New Events";
                                        logger->log(spdlog::level::info, crow::json::dump(log_data));
                                    }
                                    return crow::response(204, response);
                                }
                            } catch (const std::out_of_range& ex) {
                                response["error"] = "Not Found - Topic Does Not Exist";
                                if (logger != nullptr) {
                                    log_data["message"] = "Topic Does Not Exist";
                                    logger->log(spdlog::level::err, crow::json::dump(log_data));
                                }
                                return crow::response(404, response);
                            }
                        } catch (const std::out_of_range& ex) {
                            response["error"] = "Conflict - No Such Active Client";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(409, response);
                        } catch (const std::invalid_argument& ex) {
                            response["error"] = "Unauthenticated - Invalid Token Supplied";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(401, response);
                        } catch (const std::runtime_error& ex) {
                            response["error"] = "Bad Request - Missing Required Information";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(400, response);
                        } catch (std::exception& ex) {
                            response["error"] = "Internal Server Error";
                            if (logger != nullptr) {
                                log_data["message"] = "Internal Server Error";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(500, response);
                        }
                    });

    /*
     * request-data:
     *      "token" - JWToken, with claim "name", expiring in 'token_validity' hours.
     * response-data:
     *      if authorized: (200: OK - Disconnected Successfully)
     *          "status" - States the status.
     *      else: (409: Conflict - No Such Active Client,
     *          401: Unauthenticated - Invalid Token Supplied,
     *          400: Bad Request - Missing Required Information,
     *          500: Internal Server Error)
     *          "error" - States the error.
     */
    CROW_ROUTE(app, "/disconnect")
            .methods("POST"_method)
                    ([this](const crow::request& req) {
                        crow::json::wvalue response;
                        crow::json::wvalue log_data;
                        auto logger = spdlog::get(logger_name);
                        if (logger != nullptr) {
                            log_data["time"] = get_current_time();
                            log_data["request"] = "DISCONNECT";
                        }
                        try {
                            auto body = crow::json::load(req.body);
                            std::string token(body["token"].s());
                            auto decoded_token = jwt::decode(token);
                            auto claims = decoded_token.get_payload_claims();
                            auto _name = claims["name"];
                            std::string name = _name.as_string();
                            Client& client = clients.at(name);
                            if (logger != nullptr) {
                                log_data["client"] = client.name();
                            }
                            for (auto& topic: topics) {
                                if (accessList->isSubscriberOf(client.name(), topic.first)) {
                                    topic.second.remove_client(client);
                                }
                            }
                            clients.erase(name);
                            response["status"] = "OK - Disconnected Successfully";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::info, crow::json::dump(log_data));
                            }
                            return crow::response(200, response);
                        } catch (const std::out_of_range& ex) {
                            response["error"] = "Conflict - No Such Active Client";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(409, response);
                        } catch (const std::invalid_argument& ex) {
                            response["error"] = "Unauthenticated - Invalid Token Supplied";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(401, response);
                        } catch (const std::runtime_error& ex) {
                            response["error"] = "Bad Request - Missing Required Information";
                            if (logger != nullptr) {
                                log_data["message"] = "Disconnected Successfully";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(400, response);
                        } catch (std::exception& ex) {
                            response["error"] = "Internal Server Error";
                            if (logger != nullptr) {
                                log_data["message"] = "Internal Server Error";
                                logger->log(spdlog::level::err, crow::json::dump(log_data));
                            }
                            return crow::response(500, response);
                        }
                    });

    app.port(port_no).multithreaded(num_api_threads).ssl_file(crt_file_path, key_file_path).run();
}

void SystemContext::AddClient(const std::string& name, const std::string& ip_address, const std::string& notif_port_no) {
    mutex_lock.lock();
    clients.insert({name, Client(name, ip_address, notif_port_no)});
    mutex_lock.unlock();
}
