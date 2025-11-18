#pragma once

namespace common::logger
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };

    class Logger
    {
    public:
        virtual ~Logger() = default;

        void debug(const char *message) { log(LogLevel::Debug, message); }
        void info(const char *message) { log(LogLevel::Info, message); }
        void warning(const char *message) { log(LogLevel::Warning, message); }
        void error(const char *message) { log(LogLevel::Error, message); }
        void critical(const char *message) { log(LogLevel::Critical, message); }

    protected:
        virtual void log(LogLevel level, const char *message) = 0;
    };
}