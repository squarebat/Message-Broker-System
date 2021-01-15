#ifndef MESSAGE_BROKER_SYSTEM_EVENT_H
#define MESSAGE_BROKER_SYSTEM_EVENT_H

#include <string>
using namespace std;
class Event {
private:
    
    string message;
    Event() {}

public:
    Event(string message) 
    {
        this->message = message;
    }
};

#endif 