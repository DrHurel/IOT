#pragma once

#include "libs/plant_nanny/services/dev/IDevConfigStrategy.h"

namespace plant_nanny::services::dev
{
    /**
     * @brief Release/Production configuration strategy
     * 
     * Null Object pattern - does nothing in release builds.
     * Follows Open/Closed principle: no modification needed for release behavior.
     */
    class ReleaseConfigStrategy : public IDevConfigStrategy
    {
    public:
        void apply([[maybe_unused]] config::ConfigManager& configManager) override
        {
            // No-op in release builds
        }
        
        void logStatus() const override
        {
            // No-op in release builds
        }
        
        bool isDevMode() const override
        {
            return false;
        }
    };

} // namespace plant_nanny::services::dev
