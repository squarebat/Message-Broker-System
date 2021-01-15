#include "crow_all.h"
#include "Topic.h"
int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/auth")
    .methods("POST"_method)
    ([](){
        // TODO: Authenticate Client
        return "Client Authenticated\n";
    });

    CROW_ROUTE(app, "/create_topic/<string>")
    .methods("POST"_method)
    ([](const std::string& topic_name){
        create_topic(topic_name);
        return "New topic " + topic_name + "added\n.";
    });

    CROW_ROUTE(app, "/pub/<string>")
    .methods("POST"_method)
    ([](const std::string& topic_name){
        // TODO: Publish Message in JSON object to topic topic_name
        return "Publish on topic " + topic_name + " successful\n";
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