/**
 * @file Logger.cpp
 * @brief c++ file for the Logger class, which provides a custom logging solution using spdlog.
 *
 * This file contains the declaration of the Logger class, which sets up logging
 * using spdlog with both console and file sinks. It also defines convenience
 * macros for different logging levels.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-20
 */
#include "Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

void Logger::init() {
    try {
        // Create a console logger
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::info);

        // Create a file logger
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/logfile.log", true);
        file_sink->set_level(spdlog::level::trace);

        // Create the logger with both sinks
        spdlog::sinks_init_list sink_list = {console_sink, file_sink};
        logger = std::make_shared<spdlog::logger>("logger", sink_list.begin(), sink_list.end());
        spdlog::register_logger(logger);

        // Set the logging level
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::trace);
    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

std::shared_ptr<spdlog::logger>& Logger::getLogger() {
    return logger;
}