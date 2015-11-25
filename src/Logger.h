//
// Created by ubuntu on 11/25/15.
//

#ifndef ROCKETMQ_LOGGER_H
#define ROCKETMQ_LOGGER_H

#include <spdlog/spdlog.h>
#include <vector>
#include "Mutex.h"
#include "ScopedLock.h"

class Logger {
public:

    static std::shared_ptr<spdlog::logger> get_logger();

private:
    static bool initialized;

    static void init_logger_config();
};

#endif //ROCKETMQ_LOGGER_H
