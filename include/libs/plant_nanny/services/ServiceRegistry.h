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
     * This follows the Dependency Inversion Principle - all services are registered
     * by their interfaces, allowing for easy mocking and testing.
     * 
     * IMPORTANT: Call common::service::DefaultRegistry::create() before calling this.
     */
    inline void registerServices(const ServiceConfig& config = ServiceConfig{})
    {
        // Register core services
        common::service::add<button::IButtonHandler, button::ButtonHandler>();
        common::service::add<bluetooth::IPairingManager, bluetooth::PairingManager>();
        common::service::add<captors::ISensorManager, captors::SensorManager>();
        common::service::add<config::IConfigManager, config::ConfigManager>();
        common::service::add<network::INetworkService, network::Manager>();
        common::service::add<mqtt::IMQTTService, mqtt::MQTTService>();
        common::service::add<pump::IPump, pump::Pump>(config.pumpGpioPin);
        
        // Register command handler with pump dependency
        auto* pump = common::service::DefaultRegistry::instance().get<pump::IPump>();
        common::service::add<mqtt::IMqttCommandHandler, mqtt::MqttCommandHandler>(pump);
    }

} // namespace plant_nanny::services
