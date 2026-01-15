#pragma once

#include "libs/plant_nanny/services/config/ConfigManager.h"

namespace plant_nanny::services::dev
{
    /**
     * @brief Interface for development configuration strategies
     * 
     * Follows Interface Segregation and Dependency Inversion principles.
     * Allows different configuration strategies for different build types.
     */
    class IDevConfigStrategy
    {
    public:
        virtual ~IDevConfigStrategy() = default;
        
        /**
         * @brief Apply development configuration
         * @param configManager Reference to the config manager
         */
        virtual void apply(config::ConfigManager& configManager) = 0;
        
        /**
         * @brief Log the current configuration status
         */
        virtual void logStatus() const = 0;
        
        /**
         * @brief Check if this is a development mode strategy
         * @return true if development mode is active
         */
        virtual bool isDevMode() const = 0;
    };

} // namespace plant_nanny::services::dev
