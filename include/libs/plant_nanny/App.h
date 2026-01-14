#pragma once

#include <libs/common/App.h>
#include <memory>
#include <PubSubClient.h>
#include "libs/common/patterns/Result.h"
#include <string>
#include <esp_event.h>

// Services
#include "libs/plant_nanny/services/button/ButtonHandler.h"
#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/plant_nanny/services/config/ConfigManager.h"
#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include "libs/plant_nanny/services/network/Manager.h"

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
    // Application event IDs
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
     * @brief Main application class - coordinates components
     */
    class App : public common::App, public AppContext
    {
    private:
        // Event system
        esp_event_loop_handle_t _event_loop;

        // Services
        services::button::ButtonHandler _buttonHandler;
        services::bluetooth::PairingManager _pairingManager;
        services::config::ConfigManager _configManager;
        services::network::Manager _networkManager;
        services::mqtt::MQTTService _mqttService;
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
        void setupServices();
        void setupNetwork();
        void setupMqtt();

    public:
        App();
        ~App() noexcept override;
        App(App const &) noexcept = delete;
        App(App &&) noexcept = delete;
        App &operator=(App const &) noexcept = delete;
        App &operator=(App &&) noexcept = delete;

        // common::App interface
        void initialize() override;
        void run() override;
        void shutdown() override;
        esp_err_t on(int32_t event_id, esp_event_handler_t handler, void* handler_arg = nullptr) override;
        esp_err_t emit(int32_t event_id, void* event_data = nullptr, size_t event_data_size = 0) const override;

        // AppContext interface
        ui::ScreenManager& screenManager() override { return _screenManager; }
        services::bluetooth::PairingManager& pairingManager() override { return _pairingManager; }
        services::config::ConfigManager& configManager() override { return _configManager; }
        void setCurrentPin(const std::string& pin) override { _currentPin = pin; }
        const std::string& currentPin() const override { return _currentPin; }
        ui::screens::PairingScreen& pairingScreen() override { return *_pairingScreen; }
        void requestTransition(const std::string& stateId) override { _pendingTransition = stateId; }

        // Network management
        void configure_wifi(const std::string &ssid, const std::string &password);
        bool is_network_connected() const;

        // MQTT management
        services::mqtt::MQTTService& mqttService() { return _mqttService; }

        // OTA management
        common::patterns::Result<void> perform_ota_update(const std::string &firmware_url);
    };

} // namespace plant_nanny