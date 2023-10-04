#include <CLI/CLI.hpp>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "hv/EventLoop.h"

//int main() {
//    auto logger = spdlog::stdout_color_mt("console");
//    spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
//    logger->info("Hello, {}!", "world");
//    logger->error("This is an error message");
//
//    hv::EventLoopPtr eventLoopPtr = std::make_shared<hv::EventLoop>();
//
//    hv::TimerID id = eventLoopPtr->setInterval(1000, [&logger](hv::TimerID timerID){
//        logger->info("Hello, {}!", timerID);
//    });
//
//    logger->info("Hello jjj, {}!", id);
//
//    eventLoopPtr->run();
//    return 0;
//}
int main() {
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为自纪元以来的毫秒数
    auto epoch_time = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(epoch_time).count();

    // 输出
    std::cout << "当前时间戳 (毫秒级): " << millis << std::endl;

    return 0;
}