#pragma once

#include "libs/plant_nanny/services/mqtt/IMQTTService.h"
#include "libs/common/patterns/Result.h"
#include <functional>
#include <string>

namespace plant_nanny::services::mqtt
{
    using OtaUpdateCallback = std::function<common::patterns::Result<void>(const std::string&)>;

    /**
     * @brief Interface for MQTT command handling (DIP - Dependency Inversion Principle)
     */
    class IMqttCommandHandler
    {
    public:
        virtual ~IMqttCommandHandler() = default;

        virtual void handle(const Command& cmd) = 0;
        virtual void setOtaCallback(OtaUpdateCallback callback) = 0;
    };

} // namespace plant_nanny::services::mqtt
