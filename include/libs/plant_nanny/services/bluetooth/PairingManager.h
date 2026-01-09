#pragma once

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

    /**
     * @brief WiFi credentials received via BLE
     */
    struct WifiCredentials
    {
        std::string ssid;
        std::string password;
    };

    /**
     * @brief MQTT configuration received via BLE
     */
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

    class PairingManager
    {
    private:
        PairingState _state;
        std::string _currentPin;
        PinDisplayCallback _pinDisplayCallback;
        PairingCompleteCallback _pairingCompleteCallback;
        WifiConfigCallback _wifiConfigCallback;
        MqttConfigCallback _mqttConfigCallback;
        bool _initialized;
        unsigned long _pairingStartTime;
        static constexpr unsigned long PAIRING_TIMEOUT_MS = 120000;  // 120 seconds timeout

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

        PairingManager();
        ~PairingManager();

        PairingManager(const PairingManager&) = delete;
        PairingManager& operator=(const PairingManager&) = delete;
        PairingManager(PairingManager&&) = delete;
        PairingManager& operator=(PairingManager&&) = delete;

        /**
         * @brief Initialize the Bluetooth stack for pairing
         */
        common::patterns::Result<void> initialize();

        /**
         * @brief Start pairing mode - will generate PIN and start advertising
         * @return Result with the generated PIN or error
         */
        common::patterns::Result<std::string> startPairing();

        /**
         * @brief Stop pairing mode
         */
        common::patterns::Result<void> stopPairing();

        /**
         * @brief Check and update pairing status - call in main loop
         */
        void update();

        /**
         * @brief Get current pairing state
         */
        PairingState getState() const { return _state; }

        /**
         * @brief Get current PIN (only valid during pairing)
         */
        const std::string& getCurrentPin() const { return _currentPin; }

        /**
         * @brief Check if a device is currently paired
         */
        bool isPaired() const { return _state == PairingState::PAIRED; }

        /**
         * @brief Set callback for PIN display
         */
        void setPinDisplayCallback(PinDisplayCallback callback) { _pinDisplayCallback = callback; }

        /**
         * @brief Set callback for pairing completion
         */
        void setPairingCompleteCallback(PairingCompleteCallback callback) { _pairingCompleteCallback = callback; }

        /**
         * @brief Set callback for WiFi configuration received
         */
        void setWifiConfigCallback(WifiConfigCallback callback) { _wifiConfigCallback = callback; }

        /**
         * @brief Set callback for MQTT configuration received
         */
        void setMqttConfigCallback(MqttConfigCallback callback) { _mqttConfigCallback = callback; }

        /**
         * @brief Notify that WiFi configuration was successful
         */
        void notifyWifiConfigured(bool success);

        /**
         * @brief Set the IP address to expose via BLE after WiFi connects
         */
        void setIpAddress(const std::string& ipAddress);

        /**
         * @brief Get the server ID received from the app
         */
        std::string getServerId() const;

        /**
         * @brief Disconnect and unpair current device
         */
        common::patterns::Result<void> unpair();

        /**
         * @brief Called when WiFi credentials are received via BLE
         */
        void onWifiCredentialsReceived(const std::string& ssid, const std::string& password);

        /**
         * @brief Called when MQTT config is received via BLE
         */
        void onMqttConfigReceived(const std::string& host, uint16_t port);
    };

} // namespace plant_nanny::services::bluetooth
