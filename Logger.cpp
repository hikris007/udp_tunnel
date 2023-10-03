//
// Created by Kris Allen on 2023/10/2.
//

#include "header/Logger.h"

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

std::shared_ptr<spdlog::logger> Logger::getLogger() {
    return this->_logger;
}