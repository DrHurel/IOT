#pragma once

#include "libs/plant_nanny/services/config/IConfigManager.h"
#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <string>

namespace plant_nanny::services::config
{
    class ConfigManager : public IConfigManager
    {
    private:
        bool _initialized;
        static constexpr const char* NAMESPACE = "plantnanny";

    public:
        ConfigManager();
        ~ConfigManager() override = default;

        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;
        ConfigManager(ConfigManager&&) = delete;
        ConfigManager& operator=(ConfigManager&&) = delete;

        common::patterns::Result<void> initialize() override;
        common::patterns::Result<void> factoryReset() override;
        common::patterns::Result<void> saveWifiCredentials(const std::string& ssid, const std::string& password) override;
        common::patterns::Result<std::string> getWifiSsid() override;
        common::patterns::Result<std::string> getWifiPassword() override;
        bool isConfigured() override;
        common::patterns::Result<void> setConfigured(bool configured) override;
        common::patterns::Result<void> saveMqttConfig(const std::string& host, uint16_t port = 1883) override;
        common::patterns::Result<void> saveMqttCredentials(const std::string& username, const std::string& password) override;
        common::patterns::Result<std::string> getMqttHost() override;
        uint16_t getMqttPort() override;
        common::patterns::Result<std::string> getMqttUsername() override;
        common::patterns::Result<std::string> getMqttPassword() override;
        bool isMqttConfigured() override;
        std::string getOrCreateDeviceId() override;

        std::string getDeviceId();
        common::patterns::Result<void> setDeviceId(const std::string& deviceId);

    private:
        std::string generateUUID();
    };

} // namespace plant_nanny::services::config
