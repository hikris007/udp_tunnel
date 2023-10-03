//
// Created by Kris Allen on 2023/10/2.
//

#include "header/Logger.h"

Logger::Logger()  {
    this->_logger = spdlog::stdout_color_mt("console");
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%L%$] [%s:%#] %v");
}

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

std::shared_ptr<spdlog::logger> Logger::getLogger() {
    return this->_logger;
}