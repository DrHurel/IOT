#pragma once

#include <libs/common/service/Registry.h>

// Interfaces
#include "libs/plant_nanny/services/button/IButtonHandler.h"
#include "libs/plant_nanny/services/bluetooth/IPairingManager.h"
#include "libs/plant_nanny/services/captors/ISensorManager.h"
#include "libs/plant_nanny/services/config/IConfigManager.h"
#include "libs/plant_nanny/services/network/INetworkService.h"
#include "libs/plant_nanny/services/mqtt/IMQTTService.h"
#include "libs/plant_nanny/services/mqtt/IMqttCommandHandler.h"
#include "libs/plant_nanny/services/pump/IPump.h"

// Implementations
#include "libs/plant_nanny/services/button/ButtonHandler.h"
#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/plant_nanny/services/captors/SensorManager.h"
#include "libs/plant_nanny/services/config/ConfigManager.h"
#include "libs/plant_nanny/services/network/Manager.h"
#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include "libs/plant_nanny/services/mqtt/MqttCommandHandler.h"
#include "libs/plant_nanny/services/pump/Pump.h"

namespace plant_nanny::services
{
    /**
     * @brief Configuration for service registration
     */
    struct ServiceConfig
    {
        uint8_t pumpGpioPin = 13;
    };

    /**
     * @brief Register all plant_nanny services in the common::service registry
     * 
     * Services are registered in dependency order:
     * 1. ConfigManager (no dependencies, needed by PairingManager)
     * 2. ButtonHandler (no dependencies)
     * 3. SensorManager (no dependencies, self-configures)
     * 4. NetworkManager (no dependencies)
     * 5. MQTTService (no dependencies)
     * 6. Pump (no dependencies)
     * 7. PairingManager (depends on ConfigManager for device ID)
     * 8. MqttCommandHandler (depends on Pump)
     * 
     * IMPORTANT: Call common::service::DefaultRegistry::create() before calling this.
     */
    inline void registerServices(const ServiceConfig& config = ServiceConfig{})
    {
        // 1. ConfigManager first - other services depend on it
        common::service::add<config::IConfigManager, config::ConfigManager>();
        
        // 2. Independent services (no dependencies)
        common::service::add<button::IButtonHandler, button::ButtonHandler>();
        common::service::add<captors::ISensorManager, captors::SensorManager>();
        common::service::add<network::INetworkService, network::Manager>();
        common::service::add<mqtt::IMQTTService, mqtt::MQTTService>();
        common::service::add<pump::IPump, pump::Pump>(config.pumpGpioPin);
        
        // 3. Services with dependencies
        common::service::add<bluetooth::IPairingManager, bluetooth::PairingManager>(); // depends on ConfigManager
        common::service::add<mqtt::IMqttCommandHandler, mqtt::MqttCommandHandler>();   // depends on Pump
    }

} // namespace plant_nanny::services
