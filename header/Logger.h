//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_LOGGER_H
#define UDP_TUNNEL_LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


class Logger {
public:
    static Logger& getInstance();
    std::shared_ptr<spdlog::logger> getLogger();

    // 删除复制和赋值操作
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
private:

    // 私有构造函数，确保外部无法创建新实例
    Logger();

private:
    std::shared_ptr<spdlog::logger> _logger = nullptr;
};

#define LOGGER_DEBUG(...) SPDLOG_LOGGER_DEBUG(Logger::getInstance().getLogger(), __VA_ARGS__)
#define LOGGER_INFO(...) SPDLOG_LOGGER_INFO(Logger::getInstance().getLogger(), __VA_ARGS__)
#define LOGGER_WARN(...) SPDLOG_LOGGER_WARN(Logger::getInstance().getLogger(), __VA_ARGS__)
#define LOGGER_ERROR(...) SPDLOG_LOGGER_ERROR(Logger::getInstance().getLogger(), __VA_ARGS__)


#endif //UDP_TUNNEL_LOGGER_H
