#pragma once

#include "libs/common/patterns/Result.h"
#include <string>
#include <cstdint>

namespace plant_nanny::services::config
{
    /**
     * @brief Interface for configuration management (DIP - Dependency Inversion Principle)
     */
    class IConfigManager
    {
    public:
        virtual ~IConfigManager() = default;

        virtual common::patterns::Result<void> initialize() = 0;
        virtual common::patterns::Result<void> factoryReset() = 0;

        // WiFi configuration
        virtual common::patterns::Result<void> saveWifiCredentials(const std::string& ssid, const std::string& password) = 0;
        virtual common::patterns::Result<std::string> getWifiSsid() = 0;
        virtual common::patterns::Result<std::string> getWifiPassword() = 0;

        // Device configuration
        virtual bool isConfigured() = 0;
        virtual common::patterns::Result<void> setConfigured(bool configured) = 0;
        virtual std::string getOrCreateDeviceId() = 0;

        // MQTT configuration
        virtual common::patterns::Result<void> saveMqttConfig(const std::string& host, uint16_t port = 1883) = 0;
        virtual common::patterns::Result<void> saveMqttCredentials(const std::string& username, const std::string& password) = 0;
        virtual common::patterns::Result<std::string> getMqttHost() = 0;
        virtual uint16_t getMqttPort() = 0;
        virtual common::patterns::Result<std::string> getMqttUsername() = 0;
        virtual common::patterns::Result<std::string> getMqttPassword() = 0;
        virtual bool isMqttConfigured() = 0;
    };

} // namespace plant_nanny::services::config
