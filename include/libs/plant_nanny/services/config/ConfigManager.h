#pragma once

#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <string>

namespace plant_nanny::services::config
{
    /**
     * @brief Configuration manager for persistent settings
     * 
     * Uses ESP32 Preferences (NVS) for storage
     */
    class ConfigManager
    {
    private:
        bool _initialized;
        static constexpr const char* NAMESPACE = "plantnanny";

    public:
        ConfigManager();
        ~ConfigManager() = default;

        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;
        ConfigManager(ConfigManager&&) = delete;
        ConfigManager& operator=(ConfigManager&&) = delete;

        /**
         * @brief Initialize the config manager
         */
        common::patterns::Result<void> initialize();

        /**
         * @brief Reset all configuration to factory defaults
         */
        common::patterns::Result<void> factoryReset();

        /**
         * @brief Save WiFi credentials
         */
        common::patterns::Result<void> saveWifiCredentials(const std::string& ssid, const std::string& password);

        /**
         * @brief Get saved WiFi SSID
         */
        common::patterns::Result<std::string> getWifiSsid();

        /**
         * @brief Get saved WiFi password
         */
        common::patterns::Result<std::string> getWifiPassword();

        /**
         * @brief Check if device has been configured
         */
        bool isConfigured();

        /**
         * @brief Mark device as configured
         */
        common::patterns::Result<void> setConfigured(bool configured);

        /**
         * @brief Save MQTT broker configuration
         * @param host Broker hostname or IP address
         * @param port Broker port (default 1883)
         */
        common::patterns::Result<void> saveMqttConfig(const std::string& host, uint16_t port = 1883);

        /**
         * @brief Save MQTT credentials
         * @param username MQTT username
         * @param password MQTT password
         */
        common::patterns::Result<void> saveMqttCredentials(const std::string& username, const std::string& password);

        /**
         * @brief Get saved MQTT broker host
         */
        common::patterns::Result<std::string> getMqttHost();

        /**
         * @brief Get saved MQTT broker port
         */
        uint16_t getMqttPort();

        /**
         * @brief Get saved MQTT username
         */
        common::patterns::Result<std::string> getMqttUsername();

        /**
         * @brief Get saved MQTT password
         */
        common::patterns::Result<std::string> getMqttPassword();

        /**
         * @brief Check if MQTT is configured
         */
        bool isMqttConfigured();

        /**
         * @brief Get the device ID (unique identifier for this device)
         */
        std::string getDeviceId();

        /**
         * @brief Set the device ID
         */
        common::patterns::Result<void> setDeviceId(const std::string& deviceId);

        /**
         * @brief Get or generate a unique device ID
         * If no device ID is stored (or legacy format detected), generates a new UUID v4
         */
        std::string getOrCreateDeviceId();

    private:
        /**
         * @brief Generate a random UUID v4
         */
        std::string generateUUID();
    };

} // namespace plant_nanny::services::config
