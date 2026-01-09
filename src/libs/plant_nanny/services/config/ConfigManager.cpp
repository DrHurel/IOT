#include "libs/plant_nanny/services/config/ConfigManager.h"
#include <Preferences.h>
#include <Arduino.h>
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"

namespace
{
    void log_info(const char* message)
    {
        auto logger = common::service::get<common::logger::Logger>();
        if (logger.is_available())
        {
            logger->info(message);
        }
    }
}

namespace plant_nanny::services::config
{
    static Preferences preferences;

    ConfigManager::ConfigManager()
        : _initialized(false)
    {
    }

    common::patterns::Result<void> ConfigManager::initialize()
    {
        if (_initialized)
        {
            return common::patterns::Result<void>::success();
        }

        if (!preferences.begin(NAMESPACE, false))
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Failed to initialize Preferences"));
        }

        _initialized = true;

        log_info("[CONFIG] Manager initialized");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> ConfigManager::factoryReset()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        log_info("[CONFIG] Performing factory reset...");

        // Clear all stored preferences
        preferences.clear();

        log_info("[CONFIG] Factory reset complete");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> ConfigManager::saveWifiCredentials(
        const std::string& ssid, 
        const std::string& password)
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        preferences.putString("wifi_ssid", ssid.c_str());
        preferences.putString("wifi_pass", password.c_str());

        log_info("[CONFIG] WiFi credentials saved");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<std::string> ConfigManager::getWifiSsid()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        String ssid = preferences.getString("wifi_ssid", "");
        if (ssid.length() == 0)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("WiFi SSID not configured"));
        }

        return common::patterns::Result<std::string>::success(std::string(ssid.c_str()));
    }

    common::patterns::Result<std::string> ConfigManager::getWifiPassword()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        String password = preferences.getString("wifi_pass", "");
        return common::patterns::Result<std::string>::success(std::string(password.c_str()));
    }

    bool ConfigManager::isConfigured()
    {
        if (!_initialized)
        {
            initialize();
        }

        return preferences.getBool("configured", false);
    }

    common::patterns::Result<void> ConfigManager::setConfigured(bool configured)
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        preferences.putBool("configured", configured);
        return common::patterns::Result<void>::success();
    }

} // namespace plant_nanny::services::config
