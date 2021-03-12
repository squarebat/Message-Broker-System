#include <Topic.h>
#include <utility>
#include <thread>

Topic::Topic(std::string topic_name) {
    this->name = std::move(topic_name);
    this->num_active_clients = 0;
}

Topic& Topic::operator=(const Topic& topic) {
    return *this;
}

void Topic::pub_event(std::string event) {
    mutex_lock.lock();
    count_mutex_lock.lock();
    events.emplace_back(std::move(event), num_active_clients);
    num_events_published++;
    count_mutex_lock.unlock();
    mutex_lock.unlock();
}

Event Topic::get_event_for(Client& client) {
    long index = num_events_published - client.num_events_fetched_from(name) - 1;
    Event& event = events.at(index);
    event.decrement_count();
    if (event.read_by_all_clients()) {
        std::thread thread([this] () {
            mutex_lock.lock();
            events.erase(events.begin());
            mutex_lock.unlock();
        });
        thread.detach();
    }
    client.increment_num_events_fetched_from(name);
    return event;
}

void Topic::remove_client(Client& client) {
    long index = num_events_published - client.num_events_fetched_from(name) - 1;
    long num_events;
    mutex_lock.lock();
    num_events = events.size();
    while (index <= num_events) {
        Event& event = events.at(index);
        event.decrement_count();
        if (event.read_by_all_clients()) {
            events.erase(events.begin());
        }
        index++;
    }
    decrement_num_active_clients();
    mutex_lock.unlock();
}

void Topic::increment_num_active_clients() {
    count_mutex_lock.lock();
    num_active_clients++;
    count_mutex_lock.unlock();
}

void Topic::decrement_num_active_clients() {
    count_mutex_lock.lock();
    num_active_clients--;
    count_mutex_lock.unlock();
}
