#ifndef MESSAGE_BROKER_SYSTEM_EVENT_H
#define MESSAGE_BROKER_SYSTEM_EVENT_H

#include <string>
using namespace std;
class Event {
public:
    string message;
    Event() {}
    Event(string message) 
    {
        this->message = message;
    }
};

#endif 