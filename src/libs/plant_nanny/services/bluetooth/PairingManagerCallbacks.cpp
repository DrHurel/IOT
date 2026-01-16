#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/common/logger/Log.h"
#include <NimBLEDevice.h>

namespace plant_nanny::services::bluetooth
{
    // Characteristic callback for receiving WiFi/MQTT config
    class ConfigCharacteristicCallbacks : public NimBLECharacteristicCallbacks
    {
    private:
        PairingManager& _manager;

    public:
        explicit ConfigCharacteristicCallbacks(PairingManager& manager) : _manager(manager) {}

        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override
        {
            std::string uuid = pCharacteristic->getUUID().toString();
            std::string value = pCharacteristic->getValue();

            char msg[128];
            snprintf(msg, sizeof(msg), "[BLE] Received write on %s: %s", uuid.c_str(), 
                (uuid == PairingManager::PIN_CHAR_UUID) ? "***" : value.c_str());
            LOG_INFO(msg);

            if (uuid == PairingManager::PIN_CHAR_UUID)
            {
                handlePinWrite(value);
            }
            else if (uuid == PairingManager::WIFI_PASS_CHAR_UUID)
            {
                handleWifiPassWrite(value);
            }
        }

    private:
        void handlePinWrite(const std::string& value)
        {
            if (_manager.getState() != PairingState::AWAITING_PIN) return;

            bool pinValid = _manager.verifyPin(value);
            if (pinValid)
            {
                LOG_INFO("[BLE] PIN verified successfully");
                _manager.setState(PairingState::AWAITING_WIFI_CONFIG);
                _manager.setConfigStatus(PairingManager::Status::PIN_OK);
            }
            else
            {
                LOG_INFO("[BLE] PIN verification failed");
                _manager.setConfigStatus(PairingManager::Status::PIN_INVALID);
            }
        }

        void handleWifiPassWrite(const std::string& password)
        {
            if (_manager.getState() != PairingState::AWAITING_WIFI_CONFIG) return;

            auto& chars = _manager.chars();
            std::string ssid = chars.wifiSsid ? chars.wifiSsid->getValue() : "";
            
            if (!ssid.empty() && !password.empty())
            {
                LOG_INFO("[BLE] WiFi credentials received via write callback");
                _manager.processCredentials(ssid, password);
                if (chars.mqttPassword) chars.mqttPassword->setValue("");
            }
            else
            {
                LOG_INFO("[BLE] WiFi credentials incomplete");
            }
        }
    };

    // Server callbacks for connection management
    class PairingServerCallbacks : public NimBLEServerCallbacks
    {
    private:
        PairingManager& _manager;

    public:
        explicit PairingServerCallbacks(PairingManager& manager) : _manager(manager) {}

        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override
        {
            LOG_INFO("[BLE] Device connected");
            if (_manager.getState() == PairingState::ADVERTISING)
            {
                _manager.setState(PairingState::AWAITING_PIN);
                _manager.setConfigStatus(PairingManager::Status::AWAITING_PIN);
            }
        }

        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
        {
            LOG_INFO("[BLE] Device disconnected");
            auto state = _manager.getState();
            if (state != PairingState::PAIRED && state != PairingState::IDLE)
            {
                _manager.setState(PairingState::ADVERTISING);
                pServer->startAdvertising();
            }
        }

        uint32_t onPassKeyDisplay() override { return 0; }

        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override
        {
            LOG_INFO("[BLE] Authentication complete (app-level PIN used)");
        }
    };

    // Factory functions to create callbacks
    NimBLEServerCallbacks* createServerCallbacks(PairingManager& manager)
    {
        return new PairingServerCallbacks(manager);
    }

    NimBLECharacteristicCallbacks* createCharacteristicCallbacks(PairingManager& manager)
    {
        return new ConfigCharacteristicCallbacks(manager);
    }

} // namespace plant_nanny::services::bluetooth
