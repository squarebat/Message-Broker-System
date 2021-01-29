//
// Created by mando on 23/01/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_CLIENT_H
#define MESSAGE_BROKER_SYSTEM_CLIENT_H

#include <string>
#include <utility>

class Client {
private:
    std::string _name{};
    std::string _ip_address{};
    std::string _notif_port_no{};
public:
    Client(std::string name, std::string ip_address, std::string notif_port_no):
    _name(std::move(name)), _ip_address(std::move(ip_address)), _notif_port_no(std::move(notif_port_no)) {}

    std::string& name() {
        return _name;
    }

    std::string& ip_address() {
        return _ip_address;
    }

    std::string& notif_port_no() {
        return _notif_port_no;
    }
};


#endif //MESSAGE_BROKER_SYSTEM_CLIENT_H
