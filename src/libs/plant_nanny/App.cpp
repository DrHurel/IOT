#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/LoggerFactory.h>
#include <libs/common/logger/Log.h>
#include <libs/common/service/Accessor.h>
#include <libs/common/ui/UI.h>
#include "libs/plant_nanny/ui/screens/AlreadyPairedScreen.h"
#include "libs/plant_nanny/ui/screens/WifiErrorScreen.h"
#include "libs/plant_nanny/states/NormalState.h"
#include "libs/plant_nanny/states/PairingState.h"
#include "libs/plant_nanny/states/ResettingState.h"
#include "libs/plant_nanny/services/ota/UpdateOrchestrator.h"
#include "libs/plant_nanny/services/dev/DevConfig.h"

// Define the event base for the application
ESP_EVENT_DEFINE_BASE(common::APP_EVENTS);

namespace plant_nanny
{
    App::App() : _event_loop(nullptr)
    {
        _mqtt_client = std::make_unique<PubSubClient>();
        _pairingScreen = std::make_shared<ui::screens::PairingScreen>();
        
        esp_event_loop_args_t loop_args = {
            .queue_size = 10,
            .task_name = "app_loop",
            .task_priority = uxTaskPriorityGet(NULL),
            .task_stack_size = 4096,
            .task_core_id = tskNO_AFFINITY
        };
        esp_event_loop_create(&loop_args, &_event_loop);
    }

    App::~App() noexcept
    {
        if (_event_loop != nullptr)
        {
            esp_event_loop_delete(_event_loop);
            _event_loop = nullptr;
        }
    }

    void App::setupScreens()
    {
        using namespace common::ui;
        bootstrap();

        _screenManager.registerScreen("splash", std::make_shared<ui::screens::SplashScreen>());
        _screenManager.registerScreen("normal", std::make_shared<ui::screens::NormalScreen>());
        _screenManager.registerScreen("pairing", _pairingScreen);
        _screenManager.registerScreen("success", std::make_shared<ui::screens::SuccessScreen>());
        _screenManager.registerScreen("reset", std::make_shared<ui::screens::ResetScreen>());
        _screenManager.registerScreen("wifi_config", std::make_shared<ui::screens::WifiConfigScreen>());
        _screenManager.registerScreen("config_complete", std::make_shared<ui::screens::ConfigCompleteScreen>());
        _screenManager.registerScreen("already_paired", std::make_shared<ui::screens::AlreadyPairedScreen>());
        _screenManager.registerScreen("wifi_error", std::make_shared<ui::screens::WifiErrorScreen>());
    }

    void App::setupStates()
    {
        _stateMachine.registerState(std::make_shared<states::NormalState>());
        _stateMachine.registerState(std::make_shared<states::PairingState>());
        _stateMachine.registerState(std::make_shared<states::ResettingState>());
    }

    void App::setupServices()
    {
        _buttonHandler.initialize();
        _buttonHandler.setCallback([this](services::button::ButtonEvent event) {
            _stateMachine.handleButton(*this, event);
        });
        
        _configManager.initialize();
        _pairingManager.initialize();
        
        // Set the device ID on the BLE characteristic so Flutter can read it
        std::string deviceId = _configManager.getOrCreateDeviceId();
        _pairingManager.setDeviceId(deviceId);
        
        // Apply development configuration if in debug mode
        services::dev::DevConfig::apply(_configManager);
        
        // Initialize pump on GPIO 13
        _pump.initialize(13);
        LOG_INFO("[APP] Pump initialized (GPIO 13)");
        
        // Test pump LED
        Serial.println("[APP] Turning LED ON...");
        _pump.activate();
        Serial.println("[APP] LED should be ON now, waiting 3s...");
        delay(3000);
        _pump.deactivate();
        Serial.println("[APP] LED OFF");

        _pairingManager.setStateChangeCallback([this](services::bluetooth::PairingState newState) {
            switch (newState)
            {
                case services::bluetooth::PairingState::AWAITING_WIFI_CONFIG:
                    LOG_INFO("[APP] PIN verified, showing WiFi config screen");
                    _screenManager.navigateTo("wifi_config");
                    _screenManager.render();
                    break;
                case services::bluetooth::PairingState::PAIRED:
                    LOG_INFO("[APP] Configuration complete!");
                    _screenManager.navigateTo("config_complete");
                    _screenManager.render();
                    break;
                default:
                    break;
            }
        });

        _pairingManager.setWifiConfigCallback([this](const services::bluetooth::WifiCredentials& creds) {
            LOG_INFO("[APP] WiFi credentials received via BLE");
            _configManager.saveWifiCredentials(creds.ssid, creds.password);
            _networkManager.set_credentials(creds.ssid, creds.password);
            auto result = _networkManager.connect();
            _pairingManager.notifyWifiConfigured(result.succeed());
            
            if (result.succeed())
            {
                LOG_INFO("[APP] WiFi connected via BLE config");
                _configManager.setConfigured(true);

                auto ipResult = _networkManager.get_ip_address();
                if (ipResult.succeed())
                {
                    _pairingManager.setIpAddress(ipResult.value());
                }

                setupMqtt();
            }
            else
            {
                LOG_INFO("[APP] WiFi connection failed - showing error screen");
                _screenManager.navigateTo("wifi_error");
                _screenManager.render();
                _pairingManager.setState(services::bluetooth::PairingState::AWAITING_WIFI_CONFIG);
            }
        });

        _pairingManager.setMqttConfigCallback([this](const services::bluetooth::MqttConfig& config) {
            LOG_INFO("[APP] MQTT config received via BLE");
            _configManager.saveMqttConfig(config.host, config.port);
            
            if (!config.username.empty())
            {
                _configManager.saveMqttCredentials(config.username, config.password);
            }
        });
    }

    void App::setupSensors()
    {
        // Configure sensor pins:
        // - Thermistor (thermal resistance) on pin 33
        // - LDR (light-sensitive resistance) on pin 37  
        // - Transistor power control on pin 26
        services::captors::SensorPins pins;
        pins.thermistorPin = 33;
        pins.ldrPin = 37;
        pins.powerPin = 26;
        
        _sensorManager.initialize(pins);
        
        // Configure thermistor parameters (adjust based on your specific thermistor)
        // Common 10K NTC thermistor with B=3950
        services::captors::temperature::ThermistorConfig thermConfig;
        thermConfig.nominalResistance = 10000.0f;   // 10K ohms at 25°C
        thermConfig.nominalTemperature = 25.0f;     // 25°C reference
        thermConfig.betaCoefficient = 3950.0f;      // Beta coefficient
        thermConfig.seriesResistance = 5600.0f;     // 5.6K series resistor
        
        _sensorManager.configureThermistor(thermConfig);
        
        LOG_INFO("[APP] Sensors initialized (Thermistor:33, LDR:37, Power:26)");
        
        // Test read sensors on startup
        auto testData = _sensorManager.read();
        if (testData.valid)
        {
            Serial.printf("[APP] Sensor test: Temp=%.1fC Light=%.1f%%\n", 
                testData.temperatureC, testData.luminosityPct);
        }
        else
        {
            LOG_INFO("[APP] Sensor test: invalid reading");
        }
    }

    void App::setupNetwork()
    {
        auto ssidResult = _configManager.getWifiSsid();
        auto passResult = _configManager.getWifiPassword();
        
        if (ssidResult.succeed() && !ssidResult.value().empty())
        {
            _networkManager.set_credentials(ssidResult.value(), passResult.value());
            auto connectResult = _networkManager.connect();
            if (connectResult.succeed())
            {
                LOG_INFO("[APP] WiFi connected");
            }
            else
            {
                LOG_INFO("[APP] WiFi connection failed, will retry");
            }
        }
    }

    void App::setupMqtt()
    {
        if (!_configManager.isMqttConfigured())
        {
            LOG_INFO("[APP] MQTT not configured, skipping");
            return;
        }
        
        auto hostResult = _configManager.getMqttHost();
        if (!hostResult.succeed())
        {
            return;
        }
        
        std::string deviceId = _configManager.getOrCreateDeviceId();
        uint16_t port = _configManager.getMqttPort();
        
        auto initResult = _mqttService.initialize(deviceId, hostResult.value(), port);
        if (!initResult.succeed())
        {
            LOG_INFO("[APP] MQTT init failed");
            return;
        }

        auto userResult = _configManager.getMqttUsername();
        auto passResult = _configManager.getMqttPassword();
        if (userResult.succeed() && !userResult.value().empty())
        {
            _mqttService.set_credentials(userResult.value(), passResult.value());
        }
        
        _mqttService.set_reading_callback([this]() {
            services::mqtt::SensorReading reading;
            auto sensorData = _sensorManager.read();
            if (sensorData.valid)
            {
                reading.temperatureC = sensorData.temperatureC;
                reading.luminosityPct = sensorData.luminosityPct;
                reading.humidityPct = 0.0f;  // No humidity sensor connected
            }
            else
            {
                reading.temperatureC = NAN;
                reading.luminosityPct = NAN;
                reading.humidityPct = NAN;
            }
            return reading;
        });

        _mqttService.set_command_callback([this](const services::mqtt::Command& cmd) {
            handleMqttCommand(cmd);
        });
        
        _mqttService.set_publish_interval(60000);
        _mqttService.set_enabled(true);
        
        LOG_INFO("[APP] MQTT service configured");
    }

    void App::initialize()
    {
        Serial.begin(115200);
        common::service::DefaultRegistry::create();
        
        // Use factory to register appropriate logger for build configuration
        // This follows SOLID: the factory encapsulates build-specific logic
        common::logger::LoggerFactory::registerLogger();

        setupScreens();
        setupStates();
        setupServices();
        setupSensors();
        
        // Show splash screen
        _screenManager.navigateTo("splash");
        delay(1500);

        setupNetwork();
        setupMqtt();
        _stateMachine.transitionTo("normal", *this);
        LOG_INFO("[APP] Ready");
    }

    void App::run()
    {
        _buttonHandler.poll();
        _stateMachine.update(*this);
        _networkManager.maintain_connection();
        _mqttService.update();

        if (!_pendingTransition.empty())
        {
            std::string nextState = _pendingTransition;
            _pendingTransition.clear();
            _stateMachine.transitionTo(nextState, *this);
        }
        
        delay(10);
    }

    void App::shutdown()
    {
        LOG_INFO("[APP] Shutdown");
    }

    void App::configure_wifi(const std::string &ssid, const std::string &password)
    {
        _networkManager.set_credentials(ssid, password);
        _configManager.saveWifiCredentials(ssid, password);
    }

    bool App::is_network_connected() const
    {
        return _networkManager.is_connected();
    }

    common::patterns::Result<void> App::perform_ota_update(const std::string &firmware_url)
    {
        LOG_INFO("[APP] Starting OTA update...");
        services::ota::UpdateOrchestrator orchestrator;
        auto result = orchestrator.update_from_url(firmware_url);
        if (result.succeed())
        {
            LOG_INFO("[APP] OTA update complete, restarting...");
            delay(1000);
            ESP.restart();
        }
        else
        {
            LOG_INFO("[APP] OTA update failed");
        }
        return result;
    }
    
    void App::handleMqttCommand(const services::mqtt::Command& cmd)
    {
        switch (cmd.type)
        {
            case services::mqtt::CommandType::OtaUpdate:
                if (!cmd.otaUrl.empty())
                {
                    char msg[128];
                    snprintf(msg, sizeof(msg), "[APP] OTA command received: %s", cmd.otaUrl.c_str());
                    LOG_INFO(msg);
                    perform_ota_update(cmd.otaUrl);
                }
                break;
            case services::mqtt::CommandType::Restart:
                LOG_INFO("[APP] Restart command received");
                delay(500);
                ESP.restart();
                break;
            case services::mqtt::CommandType::PumpWater:
                {
                    char msg[64];
                    snprintf(msg, sizeof(msg), "[APP] Pump water command received (%dms)", cmd.durationMs);
                    LOG_INFO(msg);
                    
                    // Activate pump for the requested duration
                    _pump.activate();
                    delay(cmd.durationMs > 0 ? cmd.durationMs : 5000);  // Default 5s if not specified
                    _pump.deactivate();
                    
                    LOG_INFO("[APP] Pump water completed");
                }
                break;
            default:
                break;
        }
    }

    esp_err_t App::on(int32_t event_id, esp_event_handler_t handler, void* handler_arg)
    {
        return esp_event_handler_register_with(_event_loop, common::APP_EVENTS, event_id, handler, handler_arg);
    }

    esp_err_t App::emit(int32_t event_id, void* event_data, size_t event_data_size) const
    {
        return esp_event_post_to(_event_loop, common::APP_EVENTS, event_id, event_data, event_data_size, portMAX_DELAY);
    }

}