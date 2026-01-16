#pragma once

#include "libs/common/patterns/Result.h"
#include <string>
#include <functional>
#include <cstdint>

namespace plant_nanny::services::mqtt
{
    struct SensorReading;
    struct Command;

    using ReadingCallback = std::function<SensorReading()>;
    using CommandCallback = std::function<void(const Command&)>;

    /**
     * @brief Interface for MQTT service (DIP - Dependency Inversion Principle)
     */
    class IMQTTService
    {
    public:
        virtual ~IMQTTService() = default;

        virtual common::patterns::Result<void> initialize(
            const std::string& deviceId,
            const std::string& broker,
            uint16_t port = 1883) = 0;

        virtual void set_credentials(const std::string& username, const std::string& password) = 0;
        virtual void set_enabled(bool enabled) = 0;
        virtual void set_publish_interval(unsigned long intervalMs) = 0;
        virtual void set_reading_callback(ReadingCallback callback) = 0;
        virtual void set_command_callback(CommandCallback callback) = 0;
        virtual void update() = 0;
        virtual bool is_connected() const = 0;
    };

} // namespace plant_nanny::services::mqtt
