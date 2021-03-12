#ifndef MESSAGE_BROKER_SYSTEM_EVENT_H
#define MESSAGE_BROKER_SYSTEM_EVENT_H

#include <string>
#include <utility>

class Event {
private:
    std::mutex mutex_lock{};
    long num_clients_to_read{};
public:
    std::string message;
    Event() = default;

    Event(std::string message, long num_clients_to_read) {
        this->message = std::move(message);
        this->num_clients_to_read = num_clients_to_read;
    }

    void decrement_count() {
        mutex_lock.lock();
        num_clients_to_read--;
        mutex_lock.unlock();
    }

    Event(Event const& event) {
        this->message = event.message;
        this->num_clients_to_read = event.num_clients_to_read;
    }

    bool read_by_all_clients() const {
        return num_clients_to_read == 0;
    }

    Event& operator=(const Event& event) {
        this->message = event.message;
        this->num_clients_to_read = event.num_clients_to_read;
        return *this;
    }
};

#endif //MESSAGE_BROKER_SYSTEM_EVENT_H
