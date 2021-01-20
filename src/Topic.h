#ifndef MESSAGE_BROKER_SYSTEM_TOPIC_H
#define MESSAGE_BROKER_SYSTEM_TOPIC_H

#include <string>
#include <list>
#include "Event.h"
#include <yaml-cpp/yaml.h>
#include <unordered_map>
using namespace std;
class Topic {
public:
    string name;
    list<Event> events{};
    Topic() {}
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
    static unordered_map<string, Topic> read_topics_from_file(string config_file_path)
    {
        unordered_map<string, Topic> topics;
        YAML::Node config = YAML::LoadFile(config_file_path);
        if(config["topics"])
        {
            for (std::size_t i=0; i < config["topics"].size(); i++)
            {
                Topic topic(config["topics"][i]["name"].as<std::string>());
                topics[topic.name] = topic;                
            }
            return topics;
        }
        cout << "Parse error" << endl;
        return topics;
    }
};

#endif 