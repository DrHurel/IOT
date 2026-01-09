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
        inline void log_pairing(const char* message)
        {
            auto logger = common::service::get<common::logger::Logger>();
            if (logger.is_available())
            {
                logger->info(message);
            }
        }
    }

    /**
     * @brief Pairing state - BLE pairing in progress
     */
    class PairingState : public IAppState
    {
    private:
        bool _pairingComplete = false;
        bool _pairingSuccess = false;

    public:
        static constexpr const char* ID = "pairing";

        const char* id() const override { return ID; }

        void onEnter(AppContext& context) override
        {
            _pairingComplete = false;
            _pairingSuccess = false;
            
            auto result = context.pairingManager().startPairing();
            if (result.succeed())
            {
                context.setCurrentPin(result.value());
                context.pairingScreen().setPin(result.value());
                context.screenManager().navigateTo("pairing");
                
                context.pairingManager().setPairingCompleteCallback([this](bool success) {
                    _pairingComplete = true;
                    _pairingSuccess = success;
                });
            }
            log_pairing("[STATE] Entered Pairing");
        }

        void onExit(AppContext& context) override
        {
            context.pairingManager().stopPairing();
            context.setCurrentPin("");
        }

        std::string handleButton(AppContext& context, services::button::ButtonEvent event) override
        {
            using namespace services::button;

            if (event == ButtonEvent::LEFT_SHORT_PRESS)
            {
                log_pairing("[APP] Cancel pair");
                return "normal";
            }
            return "";
        }

        void update(AppContext& context) override
        {
            context.pairingManager().update();
            
            if (_pairingComplete)
            {
                if (_pairingSuccess)
                {
                    context.screenManager().navigateTo("success");
                    delay(2000);
                }
                context.requestTransition("normal");
            }
        }
    };

} // namespace plant_nanny::states
