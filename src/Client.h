//
// Created by mando on 23/01/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_CLIENT_H
#define MESSAGE_BROKER_SYSTEM_CLIENT_H

#include <string>

class Client {
public:
    std::string name{};
    std::string notif_port_no{};

    Client(std::string name, std::string notif_port_no);
};


#endif //MESSAGE_BROKER_SYSTEM_CLIENT_H
