#ifndef MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H
#define MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H

#include <string>
#include <memory>
#include "AccessList.h"
#include "crow_all.h"
#include "Topic.h"
#include "Client.h"

class SystemContext {
private:
    uint16_t port_no{};
    uint16_t num_api_threads{};
    std::string crt_file_path;
    std::string key_file_path;
    std::unique_ptr<AccessList> accessList;
    crow::SimpleApp app;
    unordered_map<std::string, Topic> topics;
    unordered_map<std::string, Client> clients;

    std::mutex mutex_lock;

    SystemContext() = default;;

public:
    SystemContext(SystemContext const&) = delete;
    void operator=(SystemContext const&) = delete;
    void StartAPI();
    void AddClient(const string &ip_address, string name, string notif_port_no);
    static SystemContext & GenerateContext(int argc, char **argv);
};


#endif //MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H
