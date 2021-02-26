//
// Created by mando on 23/01/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_CLIENT_H
#define MESSAGE_BROKER_SYSTEM_CLIENT_H

#include <string>
#include <utility>
#include <map>

class Client {
private:
    std::string _name{};
    std::string _ip_address{};
    std::string _notif_port_no{};
    std::map<std::string, long> _num_events_fetched{};
public:
    Client() = default;
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

    long num_events_fetched_from(const std::string& topic_name) {
        return _num_events_fetched[topic_name];
    }

    void increment_num_events_fetched_from(const std::string& topic_name) {
        _num_events_fetched[topic_name]++;
    }
};


#endif //MESSAGE_BROKER_SYSTEM_CLIENT_H
