//
// Created by mando on 12/01/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H
#define MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H

#include <string>
#include "AccessList.h"


class SystemContext {
private:
    uint16_t port_no{};
    uint16_t num_api_threads{};
    std::string crt_file_path;
    std::string key_file_path;
    std::unique_ptr<AccessList> accessList;

    SystemContext() {};

public:
    SystemContext(SystemContext const&) = delete;
    void operator=(SystemContext const&) = delete;
    static SystemContext &GenerateContext(int argc, char **argv);
};


#endif //MESSAGE_BROKER_SYSTEM_SYSTEMCONTEXT_H
