#include "libs/plant_nanny/services/dev/DebugConfigStrategy.h"
#include <Arduino.h>

namespace plant_nanny::services::dev
{

void DebugConfigStrategy::apply(config::ConfigManager& configManager)
{
    Serial.println("[DEV] Development mode active");
    
    // Auto-configure WiFi if DEV_WIFI_SSID is defined and not empty
#if defined(DEV_WIFI_SSID) && defined(DEV_WIFI_PASS)
    const char* devSsid = DEV_WIFI_SSID;
    const char* devPass = DEV_WIFI_PASS;
    
    // Check if it's not an empty placeholder (starts with '$' means env var not resolved)
    if (devSsid && strlen(devSsid) > 0 && devSsid[0] != '$')
    {
        auto currentSsid = configManager.getWifiSsid();
        // Only set if not already configured
        if (!currentSsid.succeed() || currentSsid.value().empty())
        {
            Serial.printf("[DEV] Auto-configuring WiFi: %s\n", devSsid);
            configManager.saveWifiCredentials(devSsid, devPass);
        }
        else
        {
            Serial.printf("[DEV] WiFi already configured: %s\n", currentSsid.value().c_str());
        }
    }
#else
    Serial.println("[DEV] No WiFi credentials defined at compile time");
    (void)configManager; // Suppress unused warning when no defines
#endif

    // Auto-configure MQTT if DEV_MQTT_HOST is defined
    // Always update MQTT config in dev mode to ensure correct broker address
#if defined(DEV_MQTT_HOST)
    const char* devMqttHost = DEV_MQTT_HOST;
    
    if (devMqttHost && strlen(devMqttHost) > 0 && devMqttHost[0] != '$')
    {
        Serial.printf("[DEV] Configuring MQTT: %s:1883\n", devMqttHost);
        configManager.saveMqttConfig(devMqttHost, 1883);
        configManager.saveMqttCredentials("plantnanny_device", "device_secret_2024");
    }
#else
    Serial.println("[DEV] No MQTT host defined at compile time");
#endif
}

void DebugConfigStrategy::logStatus() const
{
    Serial.println("=================================");
    Serial.println("  PlantNanny - DEVELOPMENT MODE");
    Serial.println("=================================");
#if defined(DEV_WIFI_SSID)
    const char* ssid = DEV_WIFI_SSID;
    if (ssid && ssid[0] != '$')
    {
        Serial.printf("  WiFi SSID: %s\n", ssid);
    }
#endif
#if defined(DEV_MQTT_HOST)
    const char* host = DEV_MQTT_HOST;
    if (host && host[0] != '$')
    {
        Serial.printf("  MQTT Host: %s\n", host);
    }
#endif
    Serial.println("=================================");
}

} // namespace plant_nanny::services::dev
