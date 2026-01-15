#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <WiFi.h>
#include "libs/common/logger/Log.h"

namespace plant_nanny::services::bluetooth
{
    static NimBLECharacteristic* pConfigStatusChar = nullptr;
    static NimBLECharacteristic* pWifiSsidChar = nullptr;
    static NimBLECharacteristic* pMqttHostChar = nullptr;
    static NimBLECharacteristic* pMqttPortChar = nullptr;
    static NimBLECharacteristic* pMqttUsernameChar = nullptr;
    static NimBLECharacteristic* pMqttPasswordChar = nullptr;

    namespace
    {
        PairingManager* g_pairingManager = nullptr;
    }

    // Characteristic callback for receiving WiFi/MQTT config
    class ConfigCharacteristicCallbacks : public NimBLECharacteristicCallbacks
    {
    public:
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override
        {
            if (!g_pairingManager) return;

            std::string uuid = pCharacteristic->getUUID().toString();
            std::string value = pCharacteristic->getValue();

            char msg[128];
            snprintf(msg, sizeof(msg), "[BLE] Received write on %s: %s", uuid.c_str(), 
                (uuid == PairingManager::PIN_CHAR_UUID) ? "***" : value.c_str());
            LOG_INFO(msg);

            if (uuid == PairingManager::PIN_CHAR_UUID)
            {
                if (g_pairingManager->getState() == PairingState::AWAITING_PIN)
                {
                    bool pinValid = g_pairingManager->verifyPin(value);
                    if (pinValid)
                    {
                        LOG_INFO("[BLE] PIN verified successfully");
                        g_pairingManager->setState(PairingState::AWAITING_WIFI_CONFIG);
                        if (pConfigStatusChar)
                        {
                            pConfigStatusChar->setValue("PIN_OK");
                            pConfigStatusChar->notify();
                        }
                    }
                    else
                    {
                        LOG_INFO("[BLE] PIN verification failed");
                        if (pConfigStatusChar)
                        {
                            pConfigStatusChar->setValue("PIN_INVALID");
                            pConfigStatusChar->notify();
                        }
                    }
                }
            }
            else if (uuid == PairingManager::WIFI_PASS_CHAR_UUID)
            {
                if (g_pairingManager->getState() == PairingState::AWAITING_WIFI_CONFIG)
                {
                    std::string ssid = pWifiSsidChar ? pWifiSsidChar->getValue() : "";
                    std::string pass = value;
                    
                    if (!ssid.empty() && !pass.empty())
                    {
                        LOG_INFO("[BLE] WiFi credentials received via write callback");

                        std::string mqttHost = pMqttHostChar ? pMqttHostChar->getValue() : "";
                        std::string mqttPortStr = pMqttPortChar ? pMqttPortChar->getValue() : "1883";
                        std::string mqttUsername = pMqttUsernameChar ? pMqttUsernameChar->getValue() : "";
                        std::string mqttPassword = pMqttPasswordChar ? pMqttPasswordChar->getValue() : "";
                        uint16_t mqttPort = static_cast<uint16_t>(atoi(mqttPortStr.c_str()));
                        if (mqttPort == 0) mqttPort = 1883;
                        
                        if (!mqttHost.empty())
                        {
                            char mqttMsg[128];
                            snprintf(mqttMsg, sizeof(mqttMsg), "[BLE] MQTT config: %s:%d user=%s", 
                                mqttHost.c_str(), mqttPort, mqttUsername.c_str());
                            LOG_INFO(mqttMsg);
                            g_pairingManager->onMqttConfigReceived(mqttHost, mqttPort, mqttUsername, mqttPassword);
                        }
                        
                        g_pairingManager->handleWifiCredentials(ssid, pass);
                        if (pMqttPasswordChar) pMqttPasswordChar->setValue("");
                    }
                    else
                    {
                        LOG_INFO("[BLE] WiFi credentials incomplete");
                    }
                }
            }
        }
    };

    // Server callbacks for connection management
    class PairingServerCallbacks : public NimBLEServerCallbacks
    {
    private:
        std::string _expectedPin;

    public:
        PairingServerCallbacks(const std::string& pin) : _expectedPin(pin) {}

        void setPin(const std::string& pin) { _expectedPin = pin; }

        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override
        {
            LOG_INFO("[BLE] Device connected");
            if (g_pairingManager && g_pairingManager->getState() == PairingState::ADVERTISING)
            {
                g_pairingManager->setState(PairingState::AWAITING_PIN);
                if (pConfigStatusChar)
                {
                    pConfigStatusChar->setValue("AWAITING_PIN");
                    pConfigStatusChar->notify();
                }
            }
        }

        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
        {
            LOG_INFO("[BLE] Device disconnected");
            if (g_pairingManager)
            {
                auto state = g_pairingManager->getState();
                if (state != PairingState::PAIRED && state != PairingState::IDLE)
                {
                    g_pairingManager->setState(PairingState::ADVERTISING);
                    pServer->startAdvertising();
                }
            }
        }

        uint32_t onPassKeyDisplay() override { return 0; }

        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override
        {
            LOG_INFO("[BLE] Authentication complete (app-level PIN used)");
        }
    };

    static NimBLEServer* pServer = nullptr;
    static PairingServerCallbacks* pServerCallbacks = nullptr;
    static ConfigCharacteristicCallbacks* pCharCallbacks = nullptr;
    static NimBLECharacteristic* pWifiPassChar = nullptr;
    static NimBLECharacteristic* pDeviceIdChar = nullptr;
    static NimBLECharacteristic* pIpAddressChar = nullptr;
    static NimBLECharacteristic* pServerIdChar = nullptr;
    static NimBLECharacteristic* pWifiNetworksChar = nullptr;
    static NimBLECharacteristic* pPinChar = nullptr;

    PairingManager::PairingManager()
        : _state(PairingState::IDLE)
        , _currentPin("")
        , _pinDisplayCallback(nullptr)
        , _pairingCompleteCallback(nullptr)
        , _wifiConfigCallback(nullptr)
        , _mqttConfigCallback(nullptr)
        , _stateChangeCallback(nullptr)
        , _initialized(false)
        , _pairingStartTime(0)
    {
        g_pairingManager = this;
    }

    PairingManager::~PairingManager()
    {
        g_pairingManager = nullptr;
        if (_initialized)
        {
            stopPairing();
            NimBLEDevice::deinit(true);
        }
        if (pServerCallbacks)
        {
            delete pServerCallbacks;
            pServerCallbacks = nullptr;
        }
        if (pCharCallbacks)
        {
            delete pCharCallbacks;
            pCharCallbacks = nullptr;
        }
    }

    std::string PairingManager::generatePin()
    {
        uint32_t pin = 100000 + (esp_random() % 900000);
        char pinStr[7];
        snprintf(pinStr, sizeof(pinStr), "%06d", pin);
        return std::string(pinStr);
    }

    void PairingManager::setupServices()
    {
        NimBLEService* pConfigService = pServer->createService(CONFIG_SERVICE_UUID);

        pWifiSsidChar = pConfigService->createCharacteristic(
            WIFI_SSID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pWifiSsidChar->setCallbacks(pCharCallbacks);
        pWifiSsidChar->setValue("");

        pWifiPassChar = pConfigService->createCharacteristic(
            WIFI_PASS_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        pWifiPassChar->setCallbacks(pCharCallbacks);

        pMqttHostChar = pConfigService->createCharacteristic(
            MQTT_HOST_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pMqttHostChar->setCallbacks(pCharCallbacks);
        pMqttHostChar->setValue("");

        pMqttPortChar = pConfigService->createCharacteristic(
            MQTT_PORT_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pMqttPortChar->setCallbacks(pCharCallbacks);
        pMqttPortChar->setValue("1883");

        pMqttUsernameChar = pConfigService->createCharacteristic(
            MQTT_USERNAME_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pMqttUsernameChar->setCallbacks(pCharCallbacks);
        pMqttUsernameChar->setValue("");

        pMqttPasswordChar = pConfigService->createCharacteristic(
            MQTT_PASSWORD_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        pMqttPasswordChar->setCallbacks(pCharCallbacks);

        pConfigStatusChar = pConfigService->createCharacteristic(
            CONFIG_STATUS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        pConfigStatusChar->setValue("READY");

        pDeviceIdChar = pConfigService->createCharacteristic(
            DEVICE_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        // Use stored device ID if set, otherwise use placeholder
        pDeviceIdChar->setValue(_deviceId.empty() ? "unknown" : _deviceId);

        pIpAddressChar = pConfigService->createCharacteristic(
            IP_ADDRESS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        pIpAddressChar->setValue("");

        pServerIdChar = pConfigService->createCharacteristic(
            SERVER_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pServerIdChar->setCallbacks(pCharCallbacks);
        pServerIdChar->setValue("");

        pWifiNetworksChar = pConfigService->createCharacteristic(
            WIFI_NETWORKS_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        pWifiNetworksChar->setValue("[]");

        pPinChar = pConfigService->createCharacteristic(
            PIN_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        pPinChar->setCallbacks(pCharCallbacks);

        pConfigService->start();

        NimBLEService* pDevInfoService = pServer->createService("180A");
        NimBLECharacteristic* pManufacturer = pDevInfoService->createCharacteristic(
            "2A29",
            NIMBLE_PROPERTY::READ
        );
        pManufacturer->setValue(DEVICE_NAME);
        pDevInfoService->start();

        LOG_INFO("[BLE] Services created");
    }

    common::patterns::Result<void> PairingManager::initialize()
    {
        if (_initialized)
        {
            return common::patterns::Result<void>::success();
        }

        NimBLEDevice::init(DEVICE_NAME);
        NimBLEDevice::setSecurityAuth(false, false, false);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

        _initialized = true;
        _state = PairingState::IDLE;

        LOG_INFO("[BLE] Pairing manager initialized");

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<std::string> PairingManager::startPairing()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<std::string>::failure(initResult.error());
            }
        }

        _currentPin = generatePin();
        _pairingStartTime = millis();
        
        char msg[50];
        snprintf(msg, sizeof(msg), "[BLE] Starting pairing with PIN: %s", _currentPin.c_str());
        LOG_INFO(msg);

        pServer = NimBLEDevice::createServer();
        
        if (pServerCallbacks)
        {
            delete pServerCallbacks;
        }
        pServerCallbacks = new PairingServerCallbacks(_currentPin);
        pServer->setCallbacks(pServerCallbacks);

        if (!pCharCallbacks)
        {
            pCharCallbacks = new ConfigCharacteristicCallbacks();
        }

        setupServices();

        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->reset();
        pAdvertising->setName(DEVICE_NAME);
        pAdvertising->addServiceUUID(CONFIG_SERVICE_UUID);
        pAdvertising->addServiceUUID("180A");
        pAdvertising->enableScanResponse(true);
        pAdvertising->start();

        _state = PairingState::ADVERTISING;

        if (_pinDisplayCallback)
        {
            _pinDisplayCallback(_currentPin);
        }

        LOG_INFO("[BLE] Advertising started");

        return common::patterns::Result<std::string>::success(_currentPin);
    }

    common::patterns::Result<void> PairingManager::stopPairing()
    {
        if (_state == PairingState::IDLE)
        {
            return common::patterns::Result<void>::success();
        }

        NimBLEDevice::getAdvertising()->stop();
        
        if (pServer && pServer->getConnectedCount() > 0)
        {
            pServer->disconnect(0);
        }

        _state = PairingState::IDLE;
        _currentPin = "";

        LOG_INFO("[BLE] Pairing stopped");

        return common::patterns::Result<void>::success();
    }

    void PairingManager::update()
    {
        if (_state == PairingState::IDLE || _state == PairingState::PAIRED)
        {
            return;
        }

        if (_pairingStartTime > 0 && (millis() - _pairingStartTime) > PAIRING_TIMEOUT_MS)
        {
            LOG_INFO("[BLE] Pairing timeout");
            stopPairing();
            _state = PairingState::FAILED;
            
            if (_pairingCompleteCallback)
            {
                _pairingCompleteCallback(false);
            }
            return;
        }

        if (pServer && pServer->getConnectedCount() > 0)
        {
            NimBLEConnInfo connInfo = pServer->getPeerInfo(0);
            
            if (connInfo.isEncrypted())
            {
                if (_state == PairingState::ADVERTISING || _state == PairingState::AWAITING_PIN)
                {
                    _state = PairingState::AWAITING_WIFI_CONFIG;
                    LOG_INFO("[BLE] Waiting for WiFi configuration");
                    scanWifiNetworks();
                    
                    if (pConfigStatusChar)
                    {
                        pConfigStatusChar->setValue("AWAITING_CONFIG");
                        pConfigStatusChar->notify();
                    }
                }

                if (_state == PairingState::AWAITING_WIFI_CONFIG && pWifiSsidChar && pWifiPassChar)
                {
                    std::string ssid = pWifiSsidChar->getValue();
                    std::string pass = pWifiPassChar->getValue();
                    
                    if (!ssid.empty() && !pass.empty())
                    {
                        LOG_INFO("[BLE] WiFi credentials received");
                        _state = PairingState::CONFIGURING_WIFI;
                        
                        std::string mqttHost = pMqttHostChar ? pMqttHostChar->getValue() : "";
                        std::string mqttPortStr = pMqttPortChar ? pMqttPortChar->getValue() : "1883";
                        std::string mqttUsername = pMqttUsernameChar ? pMqttUsernameChar->getValue() : "";
                        std::string mqttPassword = pMqttPasswordChar ? pMqttPasswordChar->getValue() : "";
                        uint16_t mqttPort = static_cast<uint16_t>(atoi(mqttPortStr.c_str()));
                        if (mqttPort == 0) mqttPort = 1883;

                        if (_wifiConfigCallback)
                        {
                            WifiCredentials creds{ssid, pass};
                            _wifiConfigCallback(creds);
                        }

                        if (!mqttHost.empty() && _mqttConfigCallback)
                        {
                            MqttConfig config{mqttHost, mqttPort, mqttUsername, mqttPassword};
                            _mqttConfigCallback(config);
                        }

                        pWifiSsidChar->setValue("");
                        pWifiPassChar->setValue("");
                        if (pMqttPasswordChar) pMqttPasswordChar->setValue("");
                    }
                }
            }
            else if (_state == PairingState::ADVERTISING)
            {
                _state = PairingState::AWAITING_PIN;
            }
        }
    }

    void PairingManager::notifyWifiConfigured(bool success)
    {
        if (pConfigStatusChar)
        {
            if (success)
            {
                pConfigStatusChar->setValue("WIFI_CONFIGURED");
                setState(PairingState::PAIRED);
                LOG_INFO("[BLE] WiFi configuration successful");
                if (pServer && pServer->getConnectedCount() > 0)
                {
                    pConfigStatusChar->notify();
                }
                if (_pairingCompleteCallback)
                {
                    _pairingCompleteCallback(true);
                }
            }
            else
            {
                pConfigStatusChar->setValue("WIFI_FAILED");
                LOG_INFO("[BLE] WiFi configuration failed");
                if (pServer && pServer->getConnectedCount() > 0)
                {
                    pConfigStatusChar->notify();
                }
            }
        }
    }

    void PairingManager::handleWifiCredentials(const std::string& ssid, const std::string& password)
    {
        LOG_INFO("[BLE] Processing WiFi credentials");
        _state = PairingState::CONFIGURING_WIFI;
        
        if (_wifiConfigCallback)
        {
            WifiCredentials creds{ssid, password};
            _wifiConfigCallback(creds);
        }
    }

    void PairingManager::onWifiCredentialsReceived(const std::string& ssid, const std::string& password)
    {
        if (_wifiConfigCallback)
        {
            WifiCredentials creds{ssid, password};
            _wifiConfigCallback(creds);
        }
    }

    void PairingManager::onMqttConfigReceived(const std::string& host, uint16_t port,
                                              const std::string& username, const std::string& password)
    {
        if (_mqttConfigCallback)
        {
            MqttConfig config{host, port, username, password};
            _mqttConfigCallback(config);
        }
    }

    common::patterns::Result<void> PairingManager::unpair()
    {
        stopPairing();
        
        int numBonds = NimBLEDevice::getNumBonds();
        for (int i = 0; i < numBonds; i++)
        {
            NimBLEDevice::deleteBond(NimBLEDevice::getBondedAddress(0));
        }

        _state = PairingState::IDLE;

        LOG_INFO("[BLE] All bonds cleared");

        return common::patterns::Result<void>::success();
    }

    void PairingManager::setIpAddress(const std::string& ipAddress)
    {
        if (pIpAddressChar)
        {
            pIpAddressChar->setValue(ipAddress);
            if (pServer && pServer->getConnectedCount() > 0)
            {
                pIpAddressChar->notify();
            }
            
            char msg[64];
            snprintf(msg, sizeof(msg), "[BLE] IP address set: %s", ipAddress.c_str());
            LOG_INFO(msg);
        }
    }

    void PairingManager::setDeviceId(const std::string& deviceId)
    {
        _deviceId = deviceId;  // Store for use when services are created
        
        if (pDeviceIdChar)
        {
            pDeviceIdChar->setValue(deviceId);
        }
        
        char msg[80];
        snprintf(msg, sizeof(msg), "[BLE] Device ID set: %s", deviceId.c_str());
        LOG_INFO(msg);
    }

    std::string PairingManager::getServerId() const
    {
        if (pServerIdChar)
        {
            return pServerIdChar->getValue();
        }
        return "";
    }

    void PairingManager::scanWifiNetworks()
    {
        LOG_INFO("[BLE] Scanning for WiFi networks...");
        int numNetworks = WiFi.scanNetworks(false, false, false, 300);
        
        if (numNetworks < 0)
        {
            LOG_INFO("[BLE] WiFi scan failed");
            if (pWifiNetworksChar)
            {
                pWifiNetworksChar->setValue("[]");
            }
            return;
        }

        char msg[48];
        snprintf(msg, sizeof(msg), "[BLE] Found %d networks", numNetworks);
        LOG_INFO(msg);

        std::string json = "[";
        int count = 0;
        const int maxNetworks = 10;

        for (int i = 0; i < numNetworks && count < maxNetworks; i++)
        {
            String ssid = WiFi.SSID(i);
            if (ssid.length() == 0)
            {
                continue;
            }

            bool duplicate = false;
            for (int j = 0; j < i; j++)
            {
                if (WiFi.SSID(j) == ssid)
                {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) continue;

            if (count > 0)
            {
                json += ",";
            }

            json += "\"";
            for (size_t c = 0; c < ssid.length(); c++)
            {
                if (ssid[c] == '"')
                {
                    json += "\\\"";
                }
                else if (ssid[c] == '\\')
                {
                    json += "\\\\";
                }
                else
                {
                    json += ssid[c];
                }
            }
            json += "\"";
            count++;
        }
        json += "]";

        WiFi.scanDelete();

        if (pWifiNetworksChar)
        {
            pWifiNetworksChar->setValue(json);
            LOG_INFO("[BLE] WiFi networks list updated");
        }
    }

} // namespace plant_nanny::services::bluetooth
