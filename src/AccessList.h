#ifndef MESSAGE_BROKER_SYSTEM_ACCESSLIST_H
#define MESSAGE_BROKER_SYSTEM_ACCESSLIST_H

#include <map>
#include <vector>
#include <string>

class AccessList {
private:
    std::map<std::string, int> topics;
    std::map<std::string, int> clients;
    int** access_info;
public:
    AccessList(std::vector<std::string> topic_names, std::vector<std::string> client_names);
    ~AccessList();
    void addAsPublisherOf(const std::string& client, const std::string& topic);
    void addAsSubscriberOf(const std::string& client, const std::string& topic);
    bool isPublisherOf(const std::string& client, const std::string& topic);
    bool isSubscriberOf(const std::string& client, const std::string& topic);
};

#endif //MESSAGE_BROKER_SYSTEM_ACCESSLIST_H
