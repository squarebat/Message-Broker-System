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

    YAML::Node config = YAML::LoadFile(configFile);

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

    std::cout << systemContext.port_no << std::endl;
    std::cout << systemContext.num_api_threads << std::endl;
    std::cout << systemContext.crt_file_path << std::endl;
    std::cout << systemContext.key_file_path << std::endl;

    return systemContext;
}
