//
// Created by Kris Allen on 2023/10/2.
//

#ifndef UDP_TUNNEL_LOGGER_H
#define UDP_TUNNEL_LOGGER_H

#include "spdlog/spdlog.h"


class Logger {
public:
    // 删除复制和赋值操作
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
private:

    // 私有构造函数，确保外部无法创建新实例
    Logger() {}
};


#endif //UDP_TUNNEL_LOGGER_H
