#include "libs/plant_nanny/services/config/ConfigManager.h"
#include <Preferences.h>
#include <Arduino.h>
#include <esp_random.h>
#include "libs/common/logger/Log.h"

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
        LOG_INFO("[CONFIG] Manager initialized");
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

        LOG_INFO("[CONFIG] Performing factory reset...");
        preferences.clear();
        LOG_INFO("[CONFIG] Factory reset complete");
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
        LOG_INFO("[CONFIG] WiFi credentials saved");
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
        LOG_INFO("[CONFIG] MQTT config saved");
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
        LOG_INFO("[CONFIG] MQTT credentials saved");
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

    std::string ConfigManager::generateUUID()
    {
        // Generate UUID v4 (random) format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
        // where y is 8, 9, a, or b
        uint8_t uuid[16];
        for (int i = 0; i < 16; i++)
        {
            uuid[i] = esp_random() & 0xFF;
        }
        
        // Set version 4 (random UUID)
        uuid[6] = (uuid[6] & 0x0F) | 0x40;
        // Set variant (10xx for RFC 4122)
        uuid[8] = (uuid[8] & 0x3F) | 0x80;
        
        char uuidStr[37];  // 36 chars + null terminator
        snprintf(uuidStr, sizeof(uuidStr),
                 "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                 uuid[0], uuid[1], uuid[2], uuid[3],
                 uuid[4], uuid[5],
                 uuid[6], uuid[7],
                 uuid[8], uuid[9],
                 uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
        
        return std::string(uuidStr);
    }

    std::string ConfigManager::getOrCreateDeviceId()
    {
        std::string deviceId = getDeviceId();
        
        // UUID v4 format: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx (36 chars)
        // Regenerate if empty, wrong length, or legacy MAC-based format (esp32-...)
        bool needsRegeneration = deviceId.empty() || 
                                  deviceId.length() != 36 ||
                                  deviceId.rfind("esp32-", 0) == 0;
        
        if (needsRegeneration)
        {
            deviceId = generateUUID();
            setDeviceId(deviceId);
            LOG_INFO("[CONFIG] Generated new UUID device ID");
        }
        
        return deviceId;
    }

} // namespace plant_nanny::services::config
