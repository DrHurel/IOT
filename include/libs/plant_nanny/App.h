#pragma once

#include <libs/common/App.h>
#include <libs/common/service/Registry.h>
#include <memory>
#include <PubSubClient.h>
#include "libs/common/patterns/Result.h"
#include <string>
#include <esp_event.h>

// Service interfaces
#include "libs/plant_nanny/services/button/IButtonHandler.h"
#include "libs/plant_nanny/services/bluetooth/IPairingManager.h"
#include "libs/plant_nanny/services/captors/ISensorManager.h"
#include "libs/plant_nanny/services/config/IConfigManager.h"
#include "libs/plant_nanny/services/mqtt/IMQTTService.h"
#include "libs/plant_nanny/services/mqtt/IMqttCommandHandler.h"
#include "libs/plant_nanny/services/network/INetworkService.h"
#include "libs/plant_nanny/services/pump/IPump.h"

// UI
#include "libs/plant_nanny/ui/ScreenManager.h"
#include "libs/plant_nanny/ui/screens/SplashScreen.h"
#include "libs/plant_nanny/ui/screens/NormalScreen.h"
#include "libs/plant_nanny/ui/screens/PairingScreen.h"
#include "libs/plant_nanny/ui/screens/SuccessScreen.h"
#include "libs/plant_nanny/ui/screens/ResetScreen.h"
#include "libs/plant_nanny/ui/screens/WifiConfigScreen.h"
#include "libs/plant_nanny/ui/screens/ConfigCompleteScreen.h"

// State machine
#include "libs/plant_nanny/states/AppContext.h"
#include "libs/plant_nanny/states/StateMachine.h"

namespace plant_nanny
{
    enum AppEventId : int32_t {
        EVENT_WIFI_CONNECTED = 0,
        EVENT_WIFI_DISCONNECTED,
        EVENT_MQTT_CONNECTED,
        EVENT_MQTT_DISCONNECTED,
        EVENT_OTA_STARTED,
        EVENT_OTA_COMPLETED,
        EVENT_OTA_FAILED,
        EVENT_SENSOR_UPDATE,
        EVENT_WATERING_STARTED,
        EVENT_WATERING_COMPLETED,
    };

    /**
     * @brief Main application class using dependency injection (SOLID principles)
     * 
     * - SRP: App coordinates components, doesn't create them
     * - OCP: New services can be added without modifying App
     * - LSP: Services can be replaced with any implementation of their interface
     * - ISP: Each service interface is focused and minimal
     * - DIP: App depends on abstractions (interfaces), not concrete implementations
     */
    class App : public common::App, public AppContext
    {
    private:
        esp_event_loop_handle_t _event_loop;
        
        std::unique_ptr<PubSubClient> _mqtt_client;
        
        // UI
        ui::ScreenManager _screenManager;
        std::shared_ptr<ui::screens::PairingScreen> _pairingScreen;
        
        // State machine
        StateMachine _stateMachine;
        
        // State data
        std::string _currentPin;
        std::string _pendingTransition;

        // Initialization helpers
        void setupScreens();
        void setupStates();
        void setupAppCallbacks();
        void initMqttCallbacks();
        void tryConnectNetwork();

    public:
        /**
         * @brief Construct App (services are accessed via registry)
         */
        App();

        ~App() noexcept override;
        App(App const &) noexcept = delete;
        App(App &&) noexcept = delete;

        // common::App interface
        void initialize() override;
        void run() override;
        void shutdown() override;
        esp_err_t on(int32_t event_id, esp_event_handler_t handler, void* handler_arg = nullptr) override;
        esp_err_t emit(int32_t event_id, void* event_data = nullptr, size_t event_data_size = 0) const override;

        // AppContext interface
        ui::ScreenManager& screenManager() override { return _screenManager; }
        void setCurrentPin(const std::string& pin) override { _currentPin = pin; }
        const std::string& currentPin() const override { return _currentPin; }
        ui::screens::PairingScreen& pairingScreen() override { return *_pairingScreen; }
        void requestTransition(const std::string& stateId) override { _pendingTransition = stateId; }




        // OTA management
        common::patterns::Result<void> perform_ota_update(const std::string &firmware_url);
        
    private:
        void initEventLoop();
    };

} // namespace plant_nanny