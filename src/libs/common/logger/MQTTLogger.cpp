#include <libs/common/logger/MQTTLogger.h>
#include <string>

namespace common::logger
{
    void MQTTLogger::log(LogLevel level, const char *message)
    {
        const char *levelStr = "";
        switch (level)
        {
        case LogLevel::Debug:
            levelStr = "DEBUG";
            break;
        case LogLevel::Info:
            levelStr = "INFO";
            break;
        case LogLevel::Warning:
            levelStr = "WARNING";
            break;
        case LogLevel::Error:
            levelStr = "ERROR";
            break;
        case LogLevel::Critical:
            levelStr = "CRITICAL";
            break;
        }

        std::string formattedMessage = "[" + std::string(levelStr) + "] " + std::string(message);

        _mqtt_client->publish("logs/topic", formattedMessage.c_str());

    }
}