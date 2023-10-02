#include <CLI/CLI.hpp>
#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int main() {
    auto logger = spdlog::stdout_color_mt("console");
    spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    logger->info("Hello, {}!", "world");
    logger->error("This is an error message");
    return 0;
}