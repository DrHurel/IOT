#pragma once

#include <libs/common/logger/Logger.h>

namespace common::logger
{
    /**
     * @brief Null Object pattern implementation for Logger
     * 
     * Does nothing - used in release builds to avoid logging overhead
     * while maintaining SOLID principles (no #ifdef in client code).
     */
    class NullLogger : public Logger
    {
    public:
        NullLogger() = default;

    protected:
        void log([[maybe_unused]] LogLevel level, [[maybe_unused]] const char *message) override
        {
            // Intentionally empty - null object pattern
        }
    };
}
