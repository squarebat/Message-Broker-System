#include "StatusLog.h"
#include "Topic.h"
#include "publish_to_topic_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"
#include <string>
#include <mutex>
std::mutex mutex_lock;
StatusLog::StatusLog(std::string client_name, std::string request_content, std::string type_of_request, std::string time_of_request)
{
    this->client_name = client_name;
    this->request_content = request_content;
    this->type_of_request = type_of_request;
    this->time_of_request = time_of_request;
}
void StatusLog::LogStatus(StatusLog status_log, Topic & topic)
{
    std::string log = "";
    log = "\nClient: "+ status_log.client_name;
    log += "\nType of Request: " + status_log.type_of_request;
    log += "\n" + status_log.request_content;
    log += "\nTime Stamp: " + status_log.time_of_request;

    spdlog::init_thread_pool(8192,1);    
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[Activity Log] [%^%l%$] %v");

    auto log_publish_sink = std::make_shared<spdlog::sinks::publish_to_topic_sink_mt>(topic,log);
    log_publish_sink->set_level(spdlog::level::trace);
    
    std::size_t max_size = 1048576 * 50;
    std::size_t max_files = 1;
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/logfile.txt", 
                        max_size, max_files);
    file_sink->set_level(spdlog::level::trace);

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink, log_publish_sink};
    auto logger = std::make_shared<spdlog::async_logger>("Activity log", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    spdlog::register_logger(logger);
    logger->set_level(spdlog::level::debug);
    mutex_lock.lock();
    logger->info(log);
    mutex_lock.unlock();
}