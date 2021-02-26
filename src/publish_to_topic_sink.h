#ifndef MESSAGE_BROKER_PUBLISH_TO_TOPIC_SINK_H
#define MESSAGE_BROKER_PUBLISH_TO_TOPIC_SINK_H

#include "spdlog/sinks/base_sink.h"
#include "Topic.h"
#include "Event.h"
#include <string>
#include "spdlog/details/null_mutex.h"
#include <mutex>
#include <utility>
namespace spdlog
{
    namespace sinks
    {
        template<typename Mutex>
        class publish_to_topic_sink : public spdlog::sinks::base_sink <Mutex>
        {
            public:
                explicit publish_to_topic_sink(Topic& topic, std::string log) {
                    topic.pub_event(std::move(log));
                }
            
            protected:
                void sink_it_(const spdlog::details::log_msg & msg) override {
                    //Nothing to be done here
                }
                void flush_() override {

                }

        };

        //Multi threaded logger
        using publish_to_topic_sink_mt = publish_to_topic_sink<std::mutex>;
        //Single threaded logger
        using publish_to_topic_sink_st = publish_to_topic_sink<spdlog::details::null_mutex>;
    }
}


#endif 