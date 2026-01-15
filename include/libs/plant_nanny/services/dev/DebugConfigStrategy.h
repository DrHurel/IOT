#pragma once

#include "libs/plant_nanny/services/dev/IDevConfigStrategy.h"

namespace plant_nanny::services::dev
{
    /**
     * @brief Debug/Development configuration strategy
     * 
     * Applies development-specific configuration such as preset WiFi
     * and MQTT credentials from compile-time defines.
     */
    class DebugConfigStrategy : public IDevConfigStrategy
    {
    public:
        void apply(config::ConfigManager& configManager) override;
        void logStatus() const override;
        
        bool isDevMode() const override
        {
            return true;
        }
    };

} // namespace plant_nanny::services::dev
