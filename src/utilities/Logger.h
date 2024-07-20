/**
 * @file Logger.h
 * @brief Header file for the Logger class, which provides a custom logging solution using spdlog.
 *
 * This file contains the declaration of the Logger class, which sets up logging
 * using spdlog with both console and file sinks. It also defines convenience
 * macros for different logging levels.
 *
 * @author Nur Akmal bin Jalil
 * @date 2024-07-20
 */
#ifndef SHARKCARDGAME_LOGGER_H
#define SHARKCARDGAME_LOGGER_H

#include <spdlog/spdlog.h>

// Convenience macros for logging at different levels.
#define LOG_TRACE(...)    Logger::getLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    Logger::getLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     Logger::getLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Logger::getLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    Logger::getLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::getLogger()->critical(__VA_ARGS__)

class Logger {
public:
    /**
   * @brief Initializes the logger with console and file sinks.
   */
    static void init();

    /**
    * @brief Gets the shared pointer to the logger instance.
    * @return Shared pointer to the spdlog logger.
    */
    static std::shared_ptr<spdlog::logger>& getLogger();
private:
    /// Shared pointer to the spdlog logger instance.
    static std::shared_ptr<spdlog::logger> logger;
};


#endif //SHARKCARDGAME_LOGGER_H
