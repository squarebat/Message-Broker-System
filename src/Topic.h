#ifndef MESSAGE_BROKER_SYSTEM_TOPIC_H
#define MESSAGE_BROKER_SYSTEM_TOPIC_H

#include <string>
#include <list>
#include "Event.h"
using namespace std;
class Topic {
private:
    
    string name;
    list<Event> events{};
    Topic() {}

public:
    Topic(string topic_name) 
    {
        this->name = topic_name;
    }
    void pub_event(Event event)
    {
        this->events.push_back(event);
    }
    Event sub_event()
    {
        Event event = events.front();
        events.pop_front();
        return event;
    }
};

Topic create_topic(string name)
{
    Topic new_topic(name);
    return new_topic;
}
#endif 