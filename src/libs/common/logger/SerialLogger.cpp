#include <libs/common/logger/SerialLogger.h>
#include <HardwareSerial.h>

namespace common::logger
{
    SerialLogger::SerialLogger() {}

    void SerialLogger::log(LogLevel level, const char *message)
    {
        // SerialLogger always logs - use NullLogger via LoggerFactory
        // for builds where logging should be disabled (SOLID: SRP)
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

        Serial.print("[");
        Serial.print(levelStr);
        Serial.print("] ");
        Serial.println(message);
    }
}