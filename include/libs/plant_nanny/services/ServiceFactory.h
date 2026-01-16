#pragma once

#include "libs/plant_nanny/services/ServiceContainer.h"
#include "libs/plant_nanny/services/button/ButtonHandler.h"
#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/plant_nanny/services/captors/SensorManager.h"
#include "libs/plant_nanny/services/config/ConfigManager.h"
#include "libs/plant_nanny/services/network/Manager.h"
#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include "libs/plant_nanny/services/pump/Pump.h"

namespace plant_nanny::services
{
    /**
     * @brief Factory for creating service containers (SRP - Single Responsibility)
     * 
     * Encapsulates the creation of all services with their default implementations.
     * This separates object creation from the App class.
     */
    class ServiceFactory
    {
    public:
        struct Config
        {
            uint8_t pumpGpioPin;
            
            Config() : pumpGpioPin(13) {}
        };

        /**
         * @brief Create a service container with default implementations
         */
        static ServiceContainer createDefault(const Config& config = Config{})
        {
            ServiceContainer container;
            
            container.buttonHandler = std::make_unique<button::ButtonHandler>();
            container.pairingManager = std::make_unique<bluetooth::PairingManager>();
            container.sensorManager = std::make_unique<captors::SensorManager>();
            container.configManager = std::make_unique<config::ConfigManager>();
            container.networkManager = std::make_unique<network::Manager>();
            container.mqttService = std::make_unique<mqtt::MQTTService>();
            container.pump = std::make_unique<pump::Pump>(config.pumpGpioPin);
            
            return container;
        }
    };

} // namespace plant_nanny::services
