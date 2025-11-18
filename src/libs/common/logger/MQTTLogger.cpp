#include <libs/common/logger/MQTTLogger.h>
#include <string>

namespace common::logger
{
    void MQTTLogger::log(LogLevel level, const char *message)
    {
        // Placeholder implementation for MQTT logging
        // In a real implementation, this would publish the log message to an MQTT topic

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

        // Format the message
        std::string formattedMessage = "[" + std::string(levelStr) + "] " + std::string(message);

        // use MQTT client to publish the message
        _mqtt_client->publish("logs/topic", formattedMessage.c_str());

    }
}