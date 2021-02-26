#ifndef MESSAGE_BROKER_STATUS_LOG_H
#define MESSAGE_BROKER_STATUS_LOG_H

#include <string>
#include "Topic.h"
#include "Event.h"
#include "publish_to_topic_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

class StatusLog {        
    public:    
        std::string client_name;
        std::string request_content;
        std::string type_of_request;
        std::string time_of_request;
        StatusLog(std::string client_name, std::string request_content, std::string type_of_request, std::string time_of_request);
        static void LogStatus(StatusLog status_log, Topic & topic);
};

#endif 