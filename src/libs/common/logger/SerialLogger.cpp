#include <libs/common/logger/SerialLogger.h>

#ifdef DEBUG
#include <HardwareSerial.h>
#endif

namespace common::logger
{
    SerialLogger::SerialLogger(){}

    void SerialLogger::log(LogLevel level, const char *message)
    {
#ifdef DEBUG
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
#else
        // In production builds, this function body is empty
        // The compiler will optimize it away completely (zero overhead)
        (void)level;   // Suppress unused parameter warning
        (void)message; // Suppress unused parameter warning
#endif
    }
}