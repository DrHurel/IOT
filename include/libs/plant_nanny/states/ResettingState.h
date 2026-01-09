#pragma once

#include "libs/plant_nanny/states/IAppState.h"
#include "libs/plant_nanny/states/AppContext.h"
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"
#include <Arduino.h>

namespace plant_nanny::states
{
    namespace
    {
        inline void log_reset(const char* message)
        {
            auto logger = common::service::get<common::logger::Logger>();
            if (logger.is_available())
            {
                logger->info(message);
            }
        }
    }

    /**
     * @brief Resetting state - factory reset in progress
     */
    class ResettingState : public IAppState
    {
    public:
        static constexpr const char* ID = "resetting";

        const char* id() const override { return ID; }

        void onEnter(AppContext& context) override
        {
            context.screenManager().navigateTo("reset");
            log_reset("[STATE] Entered Resetting");
            
            // Perform reset
            context.configManager().factoryReset();
            context.pairingManager().unpair();
            
            delay(2000);
            ESP.restart();
        }

        void onExit(AppContext& context) override
        {
            // Never called - we restart
        }

        std::string handleButton(AppContext& context, services::button::ButtonEvent event) override
        {
            // Ignore buttons during reset
            return "";
        }

        void update(AppContext& context) override
        {
            // Nothing to update - we're restarting
        }
    };

} // namespace plant_nanny::states
