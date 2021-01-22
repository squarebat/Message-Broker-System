#ifndef MESSAGE_BROKER_SYSTEM_TOPIC_H
#define MESSAGE_BROKER_SYSTEM_TOPIC_H

#include <string>
#include <list>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <utility>
#include <mutex>
#include "Event.h"


class Topic {
private:
    std::mutex mutex_lock;
public:
    std::string name;
    list<Event> events{};
    Topic() = default;
    explicit Topic(string topic_name);
    Topic& operator=(const Topic& topic);
    void pub_event(Event event);
    Event get_event() const;
};

#endif