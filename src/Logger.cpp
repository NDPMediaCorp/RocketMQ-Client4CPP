//
// Created by ubuntu on 11/25/15.
//

#include "Logger.h"

bool Logger::initialized = false;

void Logger::init_logger_config() {
    try {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("/dianyi/log/rocketmq_client", "log", 1024 * 1024 * 5, 3, true));
        auto combined_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
        spdlog::register_logger(combined_logger);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log failed: " << ex.what() << std::endl;
    }
}

std::shared_ptr<spdlog::logger> Logger::get_logger() {

    if (!initialized) {
        kpr::Mutex mutex;
        kpr::ScopedLock<kpr::Mutex> scopedLock(mutex);
        if (!initialized) {
            init_logger_config();
            initialized = true;
        }
    }
    return spdlog::get("logger");
}