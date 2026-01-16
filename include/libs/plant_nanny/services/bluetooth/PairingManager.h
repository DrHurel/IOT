#pragma once

#include "libs/plant_nanny/services/bluetooth/IPairingManager.h"
#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <functional>
#include <string>
#include <cstdint>

namespace plant_nanny::services::bluetooth
{
    enum class PairingState
    {
        IDLE,
        ADVERTISING,
        AWAITING_PIN,
        CONNECTED,
        AWAITING_WIFI_CONFIG,
        CONFIGURING_WIFI,
        PAIRED,
        FAILED
    };

    struct WifiCredentials
    {
        std::string ssid;
        std::string password;
    };

    struct MqttConfig
    {
        std::string host;
        uint16_t port;
        std::string username;
        std::string password;
    };

    using PinDisplayCallback = std::function<void(const std::string& pin)>;
    using PairingCompleteCallback = std::function<void(bool success)>;
    using WifiConfigCallback = std::function<void(const WifiCredentials& creds)>;
    using MqttConfigCallback = std::function<void(const MqttConfig& config)>;
    using StateChangeCallback = std::function<void(PairingState newState)>;

    class PairingManager : public IPairingManager
    {
    private:
        PairingState _state;
        std::string _currentPin;
        std::string _deviceId;  // Device ID to expose via BLE
        PinDisplayCallback _pinDisplayCallback;
        PairingCompleteCallback _pairingCompleteCallback;
        WifiConfigCallback _wifiConfigCallback;
        MqttConfigCallback _mqttConfigCallback;
        StateChangeCallback _stateChangeCallback;
        bool _initialized;
        unsigned long _pairingStartTime;
        static constexpr unsigned long PAIRING_TIMEOUT_MS = 120000;

        /**
         * @brief Generate a random 6-digit PIN
         */
        std::string generatePin();

        /**
         * @brief Setup BLE services and characteristics
         */
        void setupServices();

    public:
        static constexpr const char* DEVICE_NAME = "PlantNanny";
        
        // Service UUIDs
        static constexpr const char* CONFIG_SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
        static constexpr const char* WIFI_SSID_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef1";
        static constexpr const char* WIFI_PASS_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef2";
        static constexpr const char* MQTT_HOST_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef3";
        static constexpr const char* MQTT_PORT_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef4";
        static constexpr const char* CONFIG_STATUS_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef5";
        static constexpr const char* DEVICE_ID_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef6";
        static constexpr const char* IP_ADDRESS_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef7";
        static constexpr const char* SERVER_ID_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef8";
        static constexpr const char* WIFI_NETWORKS_CHAR_UUID = "12345678-1234-5678-1234-56789abcdef9";
        static constexpr const char* PIN_CHAR_UUID = "12345678-1234-5678-1234-56789abcdefa";
        static constexpr const char* MQTT_USERNAME_CHAR_UUID = "12345678-1234-5678-1234-56789abcdefb";
        static constexpr const char* MQTT_PASSWORD_CHAR_UUID = "12345678-1234-5678-1234-56789abcdefc";

        PairingManager();
        ~PairingManager() override;

        PairingManager(const PairingManager&) = delete;
        PairingManager& operator=(const PairingManager&) = delete;
        PairingManager(PairingManager&&) = delete;
        PairingManager& operator=(PairingManager&&) = delete;

        // IPairingManager interface
        common::patterns::Result<void> initialize() override;
        common::patterns::Result<void> startPairing() override;
        common::patterns::Result<void> stopPairing() override;
        bool verifyPin(const std::string& pin) override { return pin == _currentPin; }
        PairingState getState() const override { return _state; }
        void setState(PairingState state) override;
        void setDeviceId(const std::string& deviceId) override;
        void setIpAddress(const std::string& ipAddress) override;
        void notifyWifiConfigured(bool success) override;
        void setPinDisplayCallback(PinDisplayCallback callback) override { _pinDisplayCallback = callback; }
        void setPairingCompleteCallback(PairingCompleteCallback callback) override { _pairingCompleteCallback = callback; }
        void setWifiConfigCallback(WifiConfigCallback callback) override { _wifiConfigCallback = callback; }
        void setMqttConfigCallback(MqttConfigCallback callback) override { _mqttConfigCallback = callback; }
        void setStateChangeCallback(StateChangeCallback callback) override { _stateChangeCallback = callback; }

        // Additional methods not in interface
        void update();
        const std::string& getCurrentPin() const { return _currentPin; }
        bool isPaired() const { return _state == PairingState::PAIRED; }
        void handleWifiCredentials(const std::string& ssid, const std::string& password);
        std::string getServerId() const;
        void scanWifiNetworks();
        common::patterns::Result<void> unpair();
        void onWifiCredentialsReceived(const std::string& ssid, const std::string& password);
        void onMqttConfigReceived(const std::string& host, uint16_t port, 
                                  const std::string& username, const std::string& password);
    };

} // namespace plant_nanny::services::bluetooth
