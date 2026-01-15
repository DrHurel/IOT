#pragma once

#include "libs/plant_nanny/services/config/ConfigManager.h"
#include "libs/plant_nanny/services/dev/IDevConfigStrategy.h"
#include <memory>

namespace plant_nanny::services::dev
{
    /**
     * @brief Development configuration facade
     * 
     * Uses Strategy pattern to handle build-specific configuration.
     * The actual implementation is delegated to the appropriate strategy
     * (DebugConfigStrategy or ReleaseConfigStrategy) based on build type.
     * 
     * This follows SOLID principles:
     * - SRP: Only responsible for delegating to the correct strategy
     * - OCP: New build types can be added via new strategies
     * - LSP: All strategies are substitutable
     * - ISP: Interface is minimal and focused
     * - DIP: Depends on IDevConfigStrategy abstraction
     */
    class DevConfig
    {
    public:
        /**
         * @brief Apply development configuration
         * 
         * Delegates to the appropriate strategy based on build configuration.
         * 
         * @param configManager Reference to the config manager
         */
        static void apply(config::ConfigManager& configManager);
        
        /**
         * @brief Check if running in development mode
         * @return true if DEBUG is defined
         */
        static constexpr bool isDevMode()
        {
#ifdef DEBUG
            return true;
#else
            return false;
#endif
        }
        
        /**
         * @brief Log development mode status
         */
        static void logStatus();
        
        /**
         * @brief Get the appropriate configuration strategy for the current build
         * @return Unique pointer to the configuration strategy
         */
        static std::unique_ptr<IDevConfigStrategy> createStrategy();
    };

} // namespace plant_nanny::services::dev
