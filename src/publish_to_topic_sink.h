#ifndef MESSAGE_BROKER_PUBLISH_TO_TOPIC_SINK_H
#define MESSAGE_BROKER_PUBLISH_TO_TOPIC_SINK_H

#include <spdlog/sinks/base_sink.h>
#include <Topic.h>
#include <Event.h>
#include <string>
#include <spdlog/details/null_mutex.h>
#include <mutex>
#include <utility>
#include <iostream>

namespace spdlog {
    namespace sinks {
        template<typename Mutex>
        class publish_to_topic_sink : public spdlog::sinks::base_sink <Mutex> {
        private:
            Topic _topic;
        public:
            explicit publish_to_topic_sink(std::string topic_name) {
                _topic.name = std::move(topic_name);
            }

        protected:
            void sink_it_(const spdlog::details::log_msg& msg) override {
                std::string message(msg.payload.data());
                _topic.pub_event(message);
            }

            void flush_() override {
                std::cout << std::flush;
            }
        };

        //Multi threaded logger
        using publish_to_topic_sink_mt = publish_to_topic_sink<std::mutex>;
        //Single threaded logger
        using publish_to_topic_sink_st = publish_to_topic_sink<spdlog::details::null_mutex>;
    }
}

#endif //MESSAGE_BROKER_PUBLISH_TO_TOPIC_SINK_H
