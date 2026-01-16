#pragma once

#include <memory>

namespace plant_nanny::services
{
    namespace button { class IButtonHandler; }
    namespace bluetooth { class IPairingManager; }
    namespace captors { class ISensorManager; }
    namespace config { class IConfigManager; }
    namespace network { class INetworkService; }
    namespace mqtt { class IMQTTService; }
    namespace pump { class IPump; }

    /**
     * @brief Container for all application services (DIP - Dependency Inversion)
     * 
     * This structure holds references to all service interfaces, enabling
     * dependency injection and easier testing. The App class receives this
     * container and uses it to access services through their interfaces.
     * 
     * Benefits:
     * - Services can be mocked for testing
     * - Clear separation of concerns
     * - Easy to swap implementations
     */
    struct ServiceContainer
    {
        std::unique_ptr<button::IButtonHandler> buttonHandler;
        std::unique_ptr<bluetooth::IPairingManager> pairingManager;
        std::unique_ptr<captors::ISensorManager> sensorManager;
        std::unique_ptr<config::IConfigManager> configManager;
        std::unique_ptr<network::INetworkService> networkManager;
        std::unique_ptr<mqtt::IMQTTService> mqttService;
        std::unique_ptr<pump::IPump> pump;
    };

} // namespace plant_nanny::services
