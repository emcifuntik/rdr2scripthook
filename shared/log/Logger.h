#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <filesystem>
#include <string>

namespace rdr2 {

class Logger {
public:
    static void Initialize(const std::filesystem::path& logPath) {
        try {
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            consoleSink->set_level(spdlog::level::trace);
            consoleSink->set_pattern("[%H:%M:%S] [%^%l%$] %v");

            auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                logPath.string(), true);
            fileSink->set_level(spdlog::level::trace);
            fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

            std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };
            auto logger = std::make_shared<spdlog::logger>("rdr2hook", sinks.begin(), sinks.end());
            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::info);

            spdlog::set_default_logger(logger);
            spdlog::info("Logger initialized - log file: {}", logPath.string());
        }
        catch (const spdlog::spdlog_ex& ex) {
            // Fallback to console only if file logging fails
            spdlog::error("Log initialization failed: {}", ex.what());
        }
    }

    static void Shutdown() {
        spdlog::shutdown();
    }

    // Convenience methods
    template<typename... Args>
    static void Info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Warning(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Debug(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void Trace(spdlog::format_string_t<Args...> fmt, Args&&... args) {
        spdlog::trace(fmt, std::forward<Args>(args)...);
    }
};

// Shorthand macros for easy migration
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define LOG_TRACE(...) spdlog::trace(__VA_ARGS__)

} // namespace rdr2
