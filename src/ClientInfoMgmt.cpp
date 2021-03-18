#include <iostream>
#include <boost/program_options.hpp>
#include <AuthenticationData.h>

namespace po = boost::program_options;

int main(int argc, char** argv) {
    std::string clientInfoFilePath;
    std::string action;
    std::string name;
    po::options_description description("usage: eventflowctl [options]");
    description.add_options()
            ("help,h", "shows this help message")
            ("file,f", po::value<std::string>(&clientInfoFilePath)->default_value("clientinfo"),
             "client info yaml file path")
            ("action,a", po::value<std::string>(&action),
             "action [add | del | mod]")
            ("name,n", po::value<std::string>(&name),
             "client name")
            ;

    po::variables_map variablesMap;
    try {
        po::store(po::parse_command_line(argc, argv, description), variablesMap);
    } catch (boost::wrapexcept<po::invalid_command_line_syntax>& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    }
    po::notify(variablesMap);

    if (variablesMap.count("help") || action.empty()) {
        std::cerr << description << std::endl;
        exit(1);
    } else {
        AuthenticationData authData(clientInfoFilePath);
        authData.LoadData();
        if (action == "add") {
            if (name.empty()) {
                std::cout << "Client Name: ";
                std::cin >> name;
            }
            if (authData.AuthDataExists(name)) {
                std::cout << "Client with name '" + name + "' exists.\n";
                exit(1);
            } else {
                char* _pwd = getpass("Password: ");
                std::string pwd(_pwd, strlen(_pwd));
                char* _conf_pwd = getpass("Confirm Password: ");
                std::string conf_pwd(_conf_pwd, strlen(_conf_pwd));
                if (pwd == conf_pwd) {
                    authData.AddClientAuthData(name, pwd);
                    authData.SetPasswordFileMutable();
                } else {
                    std::cout << "Passwords do not match.\n";
                    exit(1);
                }
            }
        } else if (action == "del") {
            if (name.empty()) {
                std::cout << "Client Name: ";
                std::cin >> name;
            }
            if (authData.AuthDataExists(name)) {
                authData.DeleteClientAuthData(name);
                authData.SetPasswordFileMutable();
            } else {
                std::cout << "Client with name '" + name + "' does not exist.\n";
                exit(1);
            }
        } else if (action == "mod") {
            if (name.empty()) {
                std::cout << "Client Name: ";
                std::cin >> name;
            }
            if (authData.AuthDataExists(name)) {
                char* _pwd = getpass("Password: ");
                std::string pwd(_pwd, strlen(_pwd));
                char* _conf_pwd = getpass("Confirm Password: ");
                std::string conf_pwd(_conf_pwd, strlen(_conf_pwd));
                if (pwd == conf_pwd) {
                    authData.ModifyClientAuthData(name, pwd);
                    authData.SetPasswordFileMutable();
                } else {
                    std::cout << "Passwords do not match.\n";
                    exit(1);
                }
            } else {
                std::cout << "Client with name '" + name + "' does not exist.\n";
                exit(1);
            }
        } else {
            std::cerr << "Error: action must be one of ['add', 'del', 'mod']." << std::endl;
        }
        authData.WriteData();
        authData.SetPasswordFileImmutable();
    }
    return 0;
}
