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
    };

} // namespace plant_nanny::services::config
