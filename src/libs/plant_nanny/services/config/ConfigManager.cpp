#include "libs/plant_nanny/services/config/ConfigManager.h"
#include <Preferences.h>
#include <Arduino.h>
#include <WiFi.h>
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

    // ==================== MQTT Configuration ====================

    common::patterns::Result<void> ConfigManager::saveMqttConfig(const std::string& host, uint16_t port)
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        preferences.putString("mqtt_host", host.c_str());
        preferences.putUShort("mqtt_port", port);

        log_info("[CONFIG] MQTT config saved");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> ConfigManager::saveMqttCredentials(const std::string& username, const std::string& password)
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        preferences.putString("mqtt_user", username.c_str());
        preferences.putString("mqtt_pass", password.c_str());

        log_info("[CONFIG] MQTT credentials saved");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<std::string> ConfigManager::getMqttHost()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        String host = preferences.getString("mqtt_host", "");
        if (host.length() == 0)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("MQTT host not configured"));
        }

        return common::patterns::Result<std::string>::success(std::string(host.c_str()));
    }

    uint16_t ConfigManager::getMqttPort()
    {
        if (!_initialized)
        {
            initialize();
        }

        return preferences.getUShort("mqtt_port", 1883);
    }

    common::patterns::Result<std::string> ConfigManager::getMqttUsername()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        String username = preferences.getString("mqtt_user", "");
        return common::patterns::Result<std::string>::success(std::string(username.c_str()));
    }

    common::patterns::Result<std::string> ConfigManager::getMqttPassword()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        String password = preferences.getString("mqtt_pass", "");
        return common::patterns::Result<std::string>::success(std::string(password.c_str()));
    }

    bool ConfigManager::isMqttConfigured()
    {
        auto hostResult = getMqttHost();
        return hostResult.succeed() && !hostResult.value().empty();
    }

    std::string ConfigManager::getDeviceId()
    {
        if (!_initialized)
        {
            initialize();
        }

        String deviceId = preferences.getString("device_id", "");
        return std::string(deviceId.c_str());
    }

    common::patterns::Result<void> ConfigManager::setDeviceId(const std::string& deviceId)
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return initResult;
            }
        }

        preferences.putString("device_id", deviceId.c_str());
        return common::patterns::Result<void>::success();
    }

    std::string ConfigManager::getOrCreateDeviceId()
    {
        std::string deviceId = getDeviceId();
        
        if (deviceId.empty())
        {
            // Generate device ID from MAC address
            uint8_t mac[6];
            WiFi.macAddress(mac);
            
            char macStr[18];
            snprintf(macStr, sizeof(macStr), "esp32-%02x%02x%02x%02x%02x%02x",
                     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            
            deviceId = std::string(macStr);
            setDeviceId(deviceId);
            
            log_info("[CONFIG] Generated device ID from MAC");
        }
        
        return deviceId;
    }

} // namespace plant_nanny::services::config
