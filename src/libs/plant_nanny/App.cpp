#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/LoggerFactory.h>
#include <libs/common/logger/Log.h>
#include <libs/common/ui/UI.h>
#include "libs/plant_nanny/ui/screens/AlreadyPairedScreen.h"
#include "libs/plant_nanny/ui/screens/WifiErrorScreen.h"
#include "libs/plant_nanny/states/NormalState.h"
#include "libs/plant_nanny/states/PairingState.h"
#include "libs/plant_nanny/states/ResettingState.h"
#include "libs/plant_nanny/services/ota/UpdateOrchestrator.h"
#include "libs/plant_nanny/services/dev/DevConfig.h"
#include "libs/plant_nanny/services/ServiceRegistry.h"

#include <libs/common/service/Accessor.h>

ESP_EVENT_DEFINE_BASE(common::APP_EVENTS);

namespace plant_nanny
{
    void App::initEventLoop()
    {
        esp_event_loop_args_t loop_args = {
            .queue_size = 10,
            .task_name = "app_loop",
            .task_priority = uxTaskPriorityGet(NULL),
            .task_stack_size = 4096,
            .task_core_id = tskNO_AFFINITY
        };
        esp_event_loop_create(&loop_args, &_event_loop);
    }

    App::App() 
        : _event_loop(nullptr),
          _mqtt_client(std::make_unique<PubSubClient>()),
          _pairingScreen(std::make_shared<ui::screens::PairingScreen>())
    {
        initEventLoop();
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

    void App::setupAppCallbacks()
    {
        auto buttonHandler = common::service::get<services::button::IButtonHandler>();
        auto pairingManager = common::service::get<services::bluetooth::IPairingManager>();
        auto configManager = common::service::get<services::config::IConfigManager>();
        auto mqttCommandHandler = common::service::get<services::mqtt::IMqttCommandHandler>();

        // Button callback needs state machine
        buttonHandler->setCallback([this](services::button::ButtonEvent event) {
            _stateMachine.handleButton(*this, event);
        });

        // Apply dev config
        services::dev::DevConfig::apply(configManager.get());

        // Pairing callbacks need screen manager
        pairingManager->setStateChangeCallback([this](services::bluetooth::PairingState newState) {
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

        pairingManager->setWifiConfigCallback([this](const services::bluetooth::WifiCredentials& creds) {
            LOG_INFO("[APP] WiFi credentials received via BLE");
            auto cfgMgr = common::service::get<services::config::IConfigManager>();
            auto netMgr = common::service::get<services::network::INetworkService>();
            auto pairMgr = common::service::get<services::bluetooth::IPairingManager>();
            
            cfgMgr->saveWifiCredentials(creds.ssid, creds.password);
            netMgr->set_credentials(creds.ssid, creds.password);
            auto result = netMgr->connect();
            pairMgr->notifyWifiConfigured(result.succeed());
            
            if (result.succeed())
            {
                LOG_INFO("[APP] WiFi connected via BLE config");
                cfgMgr->setConfigured(true);

                auto ipResult = netMgr->get_ip_address();
                if (ipResult.succeed())
                {
                    pairMgr->setIpAddress(ipResult.value());
                }

                initMqttCallbacks();
            }
            else
            {
                LOG_INFO("[APP] WiFi connection failed - showing error screen");
                _screenManager.navigateTo("wifi_error");
                _screenManager.render();
                pairMgr->setState(services::bluetooth::PairingState::AWAITING_WIFI_CONFIG);
            }
        });

        pairingManager->setMqttConfigCallback([](const services::bluetooth::MqttConfig& config) {
            LOG_INFO("[APP] MQTT config received via BLE");
            auto cfgMgr = common::service::get<services::config::IConfigManager>();
            cfgMgr->saveMqttConfig(config.host, config.port);
            
            if (!config.username.empty())
            {
                cfgMgr->saveMqttCredentials(config.username, config.password);
            }
        });

        // OTA callback needs App
        mqttCommandHandler->setOtaCallback([this](const std::string& url) {
            return perform_ota_update(url);
        });
    }

    void App::initMqttCallbacks()
    {
        auto configManager = common::service::get<services::config::IConfigManager>();
        auto mqttService = common::service::get<services::mqtt::IMQTTService>();
        auto mqttCommandHandler = common::service::get<services::mqtt::IMqttCommandHandler>();

        if (!configManager->isMqttConfigured())
        {
            LOG_INFO("[APP] MQTT not configured, skipping");
            return;
        }
        
        auto hostResult = configManager->getMqttHost();
        if (!hostResult.succeed())
        {
            return;
        }
        
        std::string deviceId = configManager->getOrCreateDeviceId();
        uint16_t port = configManager->getMqttPort();
        
        auto initResult = mqttService->initialize(deviceId, hostResult.value(), port);
        if (!initResult.succeed())
        {
            LOG_INFO("[APP] MQTT init failed");
            return;
        }

        auto userResult = configManager->getMqttUsername();
        auto passResult = configManager->getMqttPassword();
        if (userResult.succeed() && !userResult.value().empty())
        {
            mqttService->set_credentials(userResult.value(), passResult.value());
        }
        
        mqttService->set_reading_callback([]() {
            auto sensorMgr = common::service::get<services::captors::ISensorManager>();
            services::mqtt::SensorReading reading;
            auto sensorData = sensorMgr->read();
            if (sensorData.valid)
            {
                reading.temperatureC = sensorData.temperatureC;
                reading.luminosityPct = sensorData.luminosityPct;
                reading.humidityPct = 0.0f;
            }
            else
            {
                reading.temperatureC = NAN;
                reading.luminosityPct = NAN;
                reading.humidityPct = NAN;
            }
            return reading;
        });

        mqttService->set_command_callback([&mqttCommandHandler](const services::mqtt::Command& cmd) {
            mqttCommandHandler->handle(cmd);
        });
        
        mqttService->set_publish_interval(60000);
        mqttService->set_enabled(true);
        
        LOG_INFO("[APP] MQTT service configured");
    }

    void App::tryConnectNetwork()
    {
        auto configManager = common::service::get<services::config::IConfigManager>();
        auto networkManager = common::service::get<services::network::INetworkService>();
        
        auto ssidResult = configManager->getWifiSsid();
        auto passResult = configManager->getWifiPassword();
        
        if (ssidResult.succeed() && !ssidResult.value().empty())
        {
            networkManager->set_credentials(ssidResult.value(), passResult.value());
            auto connectResult = networkManager->connect();
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

    void App::initialize()
    {
        Serial.begin(115200);
        common::service::DefaultRegistry::create();
        common::logger::LoggerFactory::registerLogger();
        services::registerServices();

        setupScreens();
        setupStates();
        setupAppCallbacks();
        
        _screenManager.navigateTo("splash");
        delay(1500);

        tryConnectNetwork();
        initMqttCallbacks();
        _stateMachine.transitionTo("normal", *this);
        LOG_INFO("[APP] Ready");
    }

    void App::run()
    {
        common::service::DefaultRegistry::instance().get<services::button::IButtonHandler>()->poll();
        _stateMachine.update(*this);
        common::service::DefaultRegistry::instance().get<services::network::INetworkService>()->maintain_connection();
        common::service::DefaultRegistry::instance().get<services::mqtt::IMQTTService>()->update();

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

    esp_err_t App::on(int32_t event_id, esp_event_handler_t handler, void* handler_arg)
    {
        return esp_event_handler_register_with(_event_loop, common::APP_EVENTS, event_id, handler, handler_arg);
    }

    esp_err_t App::emit(int32_t event_id, void* event_data, size_t event_data_size) const
    {
        return esp_event_post_to(_event_loop, common::APP_EVENTS, event_id, event_data, event_data_size, portMAX_DELAY);
    }

}