#include "libs/plant_nanny/services/mqtt/MqttCommandHandler.h"
#include "libs/plant_nanny/services/mqtt/MQTTService.h"
#include "libs/plant_nanny/services/pump/IPump.h"
#include "libs/common/logger/Log.h"
#include <Arduino.h>
#include "libs/common/service/Accessor.h"

namespace plant_nanny::services::mqtt
{

MqttCommandHandler::MqttCommandHandler()
    : _pump(&common::service::get<pump::IPump>().get())
{
}

void MqttCommandHandler::handle(const Command& cmd)
{
    switch (cmd.type)
    {
        case CommandType::OtaUpdate:
            if (!cmd.otaUrl.empty())
            {
                char msg[128];
                snprintf(msg, sizeof(msg), "[MQTT_CMD] OTA command received: %s", cmd.otaUrl.c_str());
                LOG_INFO(msg);
                
                if (_otaCallback)
                {
                    _otaCallback(cmd.otaUrl);
                }
            }
            break;
            
        case CommandType::Restart:
            LOG_INFO("[MQTT_CMD] Restart command received");
            delay(500);
            ESP.restart();
            break;
            
        case CommandType::PumpWater:
            {
                char msg[64];
                snprintf(msg, sizeof(msg), "[MQTT_CMD] Pump water command received (%dms)", cmd.durationMs);
                LOG_INFO(msg);
                
                if (_pump)
                {
                    _pump->activate();
                    delay(cmd.durationMs > 0 ? cmd.durationMs : 5000);
                    _pump->deactivate();
                }
                
                LOG_INFO("[MQTT_CMD] Pump water completed");
            }
            break;
            
        default:
            break;
    }
}

} // namespace plant_nanny::services::mqtt
