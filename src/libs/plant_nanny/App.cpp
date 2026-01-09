#include <libs/plant_nanny/App.h>
#include <libs/common/service/Registry.h>
#include <libs/common/logger/Logger.h>
#include <libs/common/logger/SerialLogger.h>
#include <libs/common/service/Accessor.h>
#include <libs/common/ui/UI.h>

// States
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
        
        // Register all screens (OCP - add new screens without modifying App)
        _screenManager.registerScreen("splash", std::make_shared<ui::screens::SplashScreen>());
        _screenManager.registerScreen("normal", std::make_shared<ui::screens::NormalScreen>());
        _screenManager.registerScreen("pairing", _pairingScreen);
        _screenManager.registerScreen("success", std::make_shared<ui::screens::SuccessScreen>());
        _screenManager.registerScreen("reset", std::make_shared<ui::screens::ResetScreen>());
    }

    void App::setupStates()
    {
        // Register all states (OCP - add new states without modifying App)
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
    }

    void App::initialize()
    {
        Serial.begin(115200);
        
        // Create service registry first (before any logging)
        common::service::DefaultRegistry::create();
#ifdef DEBUG
        common::service::add<common::logger::Logger, common::logger::SerialLogger>();
#endif

        // Setup components (SRP - each method has single responsibility)
        setupScreens();
        setupStates();
        setupServices();
        
        // Show splash screen
        _screenManager.navigateTo("splash");
        delay(1500);

        // Transition to normal state
        _stateMachine.transitionTo("normal", *this);
        log_info("[APP] Ready");
    }

    void App::run()
    {
        _buttonHandler.poll();
        _stateMachine.update(*this);
        
        // Handle pending transitions (requested by states)
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
        // WiFi configuration - to be implemented when network is needed
    }

    bool App::is_network_connected() const
    {
        return false;  // Simplified for now
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
