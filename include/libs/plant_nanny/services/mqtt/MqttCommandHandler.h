#pragma once

#include "libs/plant_nanny/services/mqtt/IMqttCommandHandler.h"
#include "libs/plant_nanny/services/pump/IPump.h"

namespace plant_nanny::services::mqtt
{
    using pump::IPump;
    /**
     * @brief Handles MQTT commands by delegating to appropriate services
     * 
     * Follows SRP - only responsible for command dispatch
     * Follows DIP - depends on abstractions (IPump, callbacks)
     */
    class MqttCommandHandler : public IMqttCommandHandler
    {
    private:
        IPump* _pump;
        OtaUpdateCallback _otaCallback;

    public:
        explicit MqttCommandHandler(IPump* pump);
        ~MqttCommandHandler() override = default;

        MqttCommandHandler(const MqttCommandHandler&) = delete;
        MqttCommandHandler& operator=(const MqttCommandHandler&) = delete;
        MqttCommandHandler(MqttCommandHandler&&) = default;
        MqttCommandHandler& operator=(MqttCommandHandler&&) = default;

        void setOtaCallback(OtaUpdateCallback callback) override { _otaCallback = std::move(callback); }

        void handle(const Command& cmd) override;
    };

} // namespace plant_nanny::services::mqtt
