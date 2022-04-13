#pragma once

#ifndef _LOGGER_H
#define _LOGGER_H 1
// #define SPDLOG_COMPILED_LIB

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Logger {
    inline void console_logger_init() {
        spdlog::drop("console");
        auto console_logger = spdlog::stdout_color_mt("console");
        console_logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
        spdlog::set_default_logger(console_logger);
    }

    inline void file_logger_init(const std::string& log_file_name) {
        spdlog::drop("file_logger");
        auto file_logger = spdlog::basic_logger_mt("file_logger", log_file_name);
        file_logger->set_pattern("[%Y-%m-%d %H:%M:%S] %v");
        file_logger->set_level(spdlog::level::trace);
        // spdlog::set_default_logger(file_logger);
    }

    inline void set_level(spdlog::level::level_enum level) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->set_level(level);
        if(file_logger) {
            file_logger->set_level(level);
        }
    }

    inline void set_pattern(const std::string& pattern) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->set_pattern(pattern);
        if(file_logger) {
            file_logger->set_pattern(pattern);
        }
    }

    inline void flush() {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->flush();
        if(file_logger) {
            file_logger->flush();
        }
    }

    template<typename... Args>
    inline void debug(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->debug(fmt, args...);
        if(file_logger) {
            file_logger->debug(fmt, args...);
        }
        flush();
    }

    template<typename... Args>
    inline void info(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->info(fmt, args...);
        if(file_logger) {
            file_logger->info(fmt, args...);
        }
        flush();
    }

    template<typename... Args>
    inline void warn(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->warn(fmt, args...);
        if(file_logger) {
            file_logger->warn(fmt, args...);
        }
        flush();
    }

    template<typename... Args>
    inline void error(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->error(fmt, args...);
        if(file_logger) {
            file_logger->error(fmt, args...);
        }
        flush();
    }

    template<typename... Args>
    inline void critical(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->critical(fmt, args...);
        if(file_logger) {
            file_logger->critical(fmt, args...);
        }
        flush();
    }

    template<typename... Args>
    inline void trace(const std::string& fmt, const Args&... args) {
        auto console_logger = spdlog::get("console");
        auto file_logger = spdlog::get("file_logger");
        console_logger->trace(fmt, args...);
        if(file_logger) {
            file_logger->trace(fmt, args...);
        }
        flush();
    }
}

#endif // _LOGGER_H