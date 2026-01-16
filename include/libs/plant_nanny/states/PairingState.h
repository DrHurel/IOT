#pragma once

#include "libs/plant_nanny/states/IAppState.h"
#include "libs/plant_nanny/states/AppContext.h"
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"
#include "libs/plant_nanny/services/config/IConfigManager.h"
#include "libs/plant_nanny/services/bluetooth/IPairingManager.h"
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
        bool _alreadyPaired = false;

    public:
        static constexpr const char* ID = "pairing";

        const char* id() const override { return ID; }

        void onEnter(AppContext& context) override
        {
            _pairingComplete = false;
            _pairingSuccess = false;
            _alreadyPaired = false;
            
            auto configManager = common::service::get<services::config::IConfigManager>();
            auto pairingManager = common::service::get<services::bluetooth::IPairingManager>();
            
            // Check if device is already configured - require factory reset to re-pair
            if (configManager->isConfigured())
            {
                log_pairing("[STATE] Device already paired - showing reset required screen");
                context.screenManager().navigateTo("already_paired");
                _alreadyPaired = true;
                return;
            }
            
            auto result = pairingManager->startPairing();
            if (result.succeed())
            {
                std::string pin = pairingManager->getCurrentPin();
                context.setCurrentPin(pin);
                context.pairingScreen().setPin(pin);
                context.screenManager().navigateTo("pairing");
                
                pairingManager->setPairingCompleteCallback([this](bool success) {
                    _pairingComplete = true;
                    _pairingSuccess = success;
                });
            }
            log_pairing("[STATE] Entered Pairing");
        }

        void onExit(AppContext& context) override
        {
            auto pairingManager = common::service::get<services::bluetooth::IPairingManager>();
            pairingManager->stopPairing();
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
            if (_alreadyPaired)
            {
                delay(3000);
                context.requestTransition("normal");
                return;
            }
            
            auto pairingManager = common::service::get<services::bluetooth::IPairingManager>();
            pairingManager->update();
            
            if (_pairingComplete)
            {
                if (_pairingSuccess)
                {
                    context.screenManager().navigateTo("success");
                    delay(5000);  // Wait for Flutter app to complete registration
                }
                context.requestTransition("normal");
            }
        }
    };

} // namespace plant_nanny::states
