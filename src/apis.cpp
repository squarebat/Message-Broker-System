#include "crow_all.h"
#include "Topic.h"
#include <unordered_map>
#include <pthread.h>
pthread_mutex_t mutex_lock;
int main()
{
    crow::SimpleApp app;
    unordered_map<string, Topic> topics = Topic::read_topics_from_file("config.yaml");
    CROW_ROUTE(app, "/auth")
    .methods("POST"_method)
    ([](){
        // TODO: Authenticate Client
        return "Client Authenticated\n";
    });

    CROW_ROUTE(app, "/publish")
    .methods("POST"_method)
    ([&](const crow::request& req)
    {
        pthread_mutex_lock(&mutex_lock);
        auto body = crow::json::load(req.body);
        if (!body["topic"] || !body["event"])
            return "Invalid Request\n";
        string topic = body["topic"].s();
        if (topics.find(topic) == topics.end())
        {
            return "Topic does not exist.\n\n";
        } 
        string msg = body["event"].s();
        Event event(msg);
        topics[topic].events.push_back(event);
        pthread_mutex_unlock(&mutex_lock);
        return "New event published on Topic\n";
    });
    
    CROW_ROUTE(app, "/sub")
    .methods("POST"_method)
    ([](){
        // TODO: Add client as a subscriber to all the topics in the JSON object
        return "Subscription Successful\n";
    });

    CROW_ROUTE(app, "/<string>")
    .methods("GET"_method)
    ([](const std::string& topic_name){
        // TODO: Fetch events from topic topic_name
        return "Event from topic " + topic_name + "\n";
    });

    CROW_ROUTE(app, "/disconnect")
    .methods("POST"_method)
    ([](){
        // TODO: Disconnect client
        return "Disconnected\n";
    });

    app.port(18080).multithreaded(20).run();
}