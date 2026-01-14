#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/SerialLogger.h>
#include <libs/common/service/Accessor.h>
#include <libs/common/ui/UI.h>
#include "libs/plant_nanny/ui/screens/AlreadyPairedScreen.h"
#include "libs/plant_nanny/ui/screens/WifiErrorScreen.h"
#include "libs/plant_nanny/states/NormalState.h"
#include "libs/plant_nanny/states/PairingState.h"
#include "libs/plant_nanny/states/ResettingState.h"

// Define the event base for the application
ESP_EVENT_DEFINE_BASE(common::APP_EVENTS);

namespace
{
    void log_info(const char* message)
    {
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->info(message);
        }
    }
}

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

        _pairingManager.setStateChangeCallback([this](services::bluetooth::PairingState newState) {
            switch (newState)
            {
                case services::bluetooth::PairingState::AWAITING_WIFI_CONFIG:
                    log_info("[APP] PIN verified, showing WiFi config screen");
                    _screenManager.navigateTo("wifi_config");
                    _screenManager.render();
                    break;
                case services::bluetooth::PairingState::PAIRED:
                    log_info("[APP] Configuration complete!");
                    _screenManager.navigateTo("config_complete");
                    _screenManager.render();
                    break;
                default:
                    break;
            }
        });

        _pairingManager.setWifiConfigCallback([this](const services::bluetooth::WifiCredentials& creds) {
            log_info("[APP] WiFi credentials received via BLE");
            _configManager.saveWifiCredentials(creds.ssid, creds.password);
            _networkManager.set_credentials(creds.ssid, creds.password);
            auto result = _networkManager.connect();
            _pairingManager.notifyWifiConfigured(result.succeed());
            
            if (result.succeed())
            {
                log_info("[APP] WiFi connected via BLE config");
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
                log_info("[APP] WiFi connection failed - showing error screen");
                _screenManager.navigateTo("wifi_error");
                _screenManager.render();
                _pairingManager.setState(services::bluetooth::PairingState::AWAITING_WIFI_CONFIG);
            }
        });

        _pairingManager.setMqttConfigCallback([this](const services::bluetooth::MqttConfig& config) {
            log_info("[APP] MQTT config received via BLE");
            _configManager.saveMqttConfig(config.host, config.port);
            
            if (!config.username.empty())
            {
                _configManager.saveMqttCredentials(config.username, config.password);
            }
        });
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
                log_info("[APP] WiFi connected");
            }
            else
            {
                log_info("[APP] WiFi connection failed, will retry");
            }
        }
    }

    void App::setupMqtt()
    {
        if (!_configManager.isMqttConfigured())
        {
            log_info("[APP] MQTT not configured, skipping");
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
            log_info("[APP] MQTT init failed");
            return;
        }

        auto userResult = _configManager.getMqttUsername();
        auto passResult = _configManager.getMqttPassword();
        if (userResult.succeed() && !userResult.value().empty())
        {
            _mqttService.set_credentials(userResult.value(), passResult.value());
        }
        
        _mqttService.set_reading_callback([]() {
            services::mqtt::SensorReading reading;
            // TODO: Get actual sensor readings
            reading.temperatureC = 22.5f;
            reading.humidityPct = 45.0f;
            reading.luminosityPct = 60.0f;
            return reading;
        });

        _mqttService.set_publish_interval(60000);
        _mqttService.set_enabled(true);
        
        log_info("[APP] MQTT service configured");
    }

    void App::initialize()
    {
        Serial.begin(115200);
        common::service::DefaultRegistry::create();
#ifdef DEBUG
        common::service::add<common::logger::Logger, common::logger::SerialLogger>();
#endif

        setupScreens();
        setupStates();
        setupServices();
        
        // Show splash screen
        _screenManager.navigateTo("splash");
        delay(1500);

        setupNetwork();
        setupMqtt();
        _stateMachine.transitionTo("normal", *this);
        log_info("[APP] Ready");
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
        log_info("[APP] Shutdown");
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
        return common::patterns::Result<void>::failure(
            common::patterns::Error("OTA not implemented"));
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
