#ifndef MESSAGE_BROKER_SYSTEM_TOPIC_H
#define MESSAGE_BROKER_SYSTEM_TOPIC_H

#include <string>
#include <list>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <utility>
#include <mutex>
#include <Event.h>
#include <Client.h>


class Topic : public basic_string<char> {
private:
    std::mutex mutex_lock;
    std::mutex count_mutex_lock;
    long num_events_published{};
    long num_active_clients{};
public:
    std::string name;
    vector<Event> events{};
    Topic() = default;
    explicit Topic(string topic_name);
    Topic& operator=(const Topic& topic);
    void pub_event(std::string event);
    Event get_event_for(Client& client);
    void increment_num_active_clients();
    void decrement_num_active_clients();
};

#endif
