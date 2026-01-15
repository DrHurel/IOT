#pragma once

#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/SerialLogger.h>
#include <libs/common/logger/NullLogger.h>

namespace common::logger
{
    /**
     * @brief Factory for creating the appropriate logger based on build configuration
     * 
     * Encapsulates build-specific logic in a single location,
     * following the Single Responsibility Principle and keeping client code
     * (like App) clean of preprocessor conditionals.
     * 
     * Supports two flags:
     * - DEBUG: Full debug build with dev config and logging
     * - SERIAL_LOGGING: Enable logging without dev config (for E2E testing)
     * 
     * This follows:
     * - SRP: Only responsible for logger creation decisions
     * - OCP: New logger types can be added without modifying client code
     * - DIP: Clients depend on Logger abstraction, not concrete implementations
     */
    class LoggerFactory
    {
    public:
        /**
         * @brief Register the appropriate logger in the service registry
         * 
         * In DEBUG or SERIAL_LOGGING builds: registers SerialLogger for console output
         * In release builds: registers NullLogger (no-op) to minimize overhead
         */
        static void registerLogger()
        {
#if defined(DEBUG) || defined(SERIAL_LOGGING)
            common::service::add<Logger, SerialLogger>();
#else
            common::service::add<Logger, NullLogger>();
#endif
        }

        /**
         * @brief Check if logging is enabled for the current build
         * @return true if DEBUG or SERIAL_LOGGING build, false otherwise
         */
        static constexpr bool isLoggingEnabled()
        {
#if defined(DEBUG) || defined(SERIAL_LOGGING)
            return true;
#else
            return false;
#endif
        }
    };
}
