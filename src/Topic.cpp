//
// Created by mando on 23/01/21.
//

#include "Topic.h"

Topic::Topic(string topic_name) {
    this->name = std::move(topic_name);
}

Topic& Topic::operator=(const Topic& topic) {
    return *this;
}

void Topic::pub_event(Event event) {
    mutex_lock.lock();
    this->events.push_back(std::move(event));
    mutex_lock.unlock();
}
Event Topic::get_event() const {
    return events.front();
}
