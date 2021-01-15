//
// Created by mando on 12/01/21.
//

#include <iostream>
#include <boost/program_options.hpp>
#include <yaml-cpp/yaml.h>
#include "SystemContext.h"

namespace po = boost::program_options;

SystemContext& SystemContext::GenerateContext(int argc, char** argv) {
    static SystemContext systemContext;

    po::options_description description("usage: eventflow [options]");
    description.add_options()
            ("help", "shows this help message")
            ("config", po::value<std::string>(),
                    "specify location of configuration file (Defaults to config.yaml)")
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

    std::string configFile("config.yaml");
    if (variablesMap.count("config")) {
        configFile.assign(variablesMap["config"].as<std::string>());
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

    const YAML::Node& topics = config["topics"];
    std::vector<std::string> topic_names;
    std::vector<std::string> client_names;

    topic_names.reserve(topics.size());

    for (const auto & topic : topics) {
        topic_names.emplace_back(topic["name"].as<std::string>());
        for (int j = 0; j < topic["publishers"].size(); j++) {
            client_names.emplace_back(topic["publishers"][j].as<std::string>());
        }
        for (int j = 0; j < topic["subscribers"].size(); j++) {
            client_names.emplace_back(topic["subscribers"][j].as<std::string>());
        }
    }

    std::unique(topic_names.begin(), topic_names.end());
    std::unique(client_names.begin(), client_names.end());

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

    return systemContext;
}
