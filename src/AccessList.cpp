//
// Created by mando on 15/01/21.
//

#include "AccessList.h"
#include <utility>


AccessList::AccessList(std::vector<std::string> topic_names, std::vector<std::string> client_names) {
    for (int i = 0; i < topic_names.size(); i++) {
        topics.insert(std::make_pair(topic_names[i], i));
    }
    for (int i = 0; i < client_names.size(); i++) {
        clients.insert(std::make_pair(client_names[i], i));
    }

    access_info = new int*[topics.size()];
    for (int i = 0; i < topics.size(); i++) {
        access_info[i] = new int[clients.size()] {0};
    }
}

AccessList::~AccessList() {
    for (int i = 0; i < topics.size(); i++) {
        delete access_info[i];
    }
    delete access_info;
}

bool AccessList::isPublisherOf(const std::string& client, const std::string& topic) {
    int _client;
    int _topic;
    try {
        _client = clients.at(client);
        _topic = topics.at(topic);
    } catch (const std::out_of_range& ex) {
        return false;
    }
    return access_info[_topic][_client] & 1;
}

bool AccessList::isSubscriberOf(const std::string& client, const std::string& topic) {
    int _client;
    int _topic;
    try {
        _client = clients.at(client);
        _topic = topics.at(topic);
    } catch (const std::out_of_range& ex) {
        return false;
    }
    return access_info[_topic][_client] & 2;
}

void AccessList::addAsPublisherOf(const std::string &client, const std::string &topic) {
    int _client = clients[client];
    int _topic = topics[topic];
    access_info[_topic][_client] |= 1;
}

void AccessList::addAsSubscriberOf(const std::string &client, const std::string &topic) {
    int _client = clients[client];
    int _topic = topics[topic];
    access_info[_topic][_client] |= 2;
}
