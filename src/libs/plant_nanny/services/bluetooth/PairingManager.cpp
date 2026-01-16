#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include "libs/plant_nanny/services/config/IConfigManager.h"
#include <libs/common/service/Registry.h>
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <WiFi.h>
#include "libs/common/logger/Log.h"
#include "libs/common/service/Accessor.h"

namespace plant_nanny::services::bluetooth
{
    namespace
    {
        constexpr uint16_t DEFAULT_MQTT_PORT = 1883;
    }

    // Factory functions declared in callbacks file
    NimBLEServerCallbacks* createServerCallbacks(PairingManager& manager);
    NimBLECharacteristicCallbacks* createCharacteristicCallbacks(PairingManager& manager);

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
        initialize();
        
        // Get device ID from ConfigManager if available
        auto configManager = common::service::get<config::IConfigManager>();
        if (configManager.is_available())
        {
            std::string deviceId = configManager->getOrCreateDeviceId();
            setDeviceId(deviceId);
        }
    }

    PairingManager::~PairingManager()
    {
        if (_initialized)
        {
            stopPairing();
            NimBLEDevice::deinit(true);
        }
        delete _pServerCallbacks;
        _pServerCallbacks = nullptr;
        delete _pCharCallbacks;
        _pCharCallbacks = nullptr;
    }

    void PairingManager::setState(PairingState state)
    {
        _state = state;
        if (_stateChangeCallback)
        {
            _stateChangeCallback(state);
        }
    }

    std::string PairingManager::generatePin()
    {
        uint32_t pin = 100000 + (esp_random() % 900000);
        char pinStr[7];
        snprintf(pinStr, sizeof(pinStr), "%06u", pin);
        return std::string(pinStr);
    }

    void PairingManager::setConfigStatus(const char* status)
    {
        if (_chars.configStatus)
        {
            _chars.configStatus->setValue(status);
            if (_pServer && _pServer->getConnectedCount() > 0)
            {
                _chars.configStatus->notify();
            }
        }
    }

    void PairingManager::processMqttConfig()
    {
        std::string mqttHost = _chars.mqttHost ? _chars.mqttHost->getValue() : "";
        if (mqttHost.empty()) return;

        std::string mqttPortStr = _chars.mqttPort ? _chars.mqttPort->getValue() : "";
        std::string mqttUsername = _chars.mqttUsername ? _chars.mqttUsername->getValue() : "";
        std::string mqttPassword = _chars.mqttPassword ? _chars.mqttPassword->getValue() : "";
        
        uint16_t mqttPort = mqttPortStr.empty() ? DEFAULT_MQTT_PORT 
                           : static_cast<uint16_t>(atoi(mqttPortStr.c_str()));
        if (mqttPort == 0) mqttPort = DEFAULT_MQTT_PORT;

        char mqttMsg[128];
        snprintf(mqttMsg, sizeof(mqttMsg), "[BLE] MQTT config: %s:%d user=%s", 
            mqttHost.c_str(), mqttPort, mqttUsername.c_str());
        LOG_INFO(mqttMsg);
        
        onMqttConfigReceived(mqttHost, mqttPort, mqttUsername, mqttPassword);
    }

    void PairingManager::processCredentials(const std::string& ssid, const std::string& pass)
    {
        processMqttConfig();
        handleWifiCredentials(ssid, pass);
    }

    void PairingManager::setupServices()
    {
        NimBLEService* pConfigService = _pServer->createService(CONFIG_SERVICE_UUID);

        _chars.wifiSsid = pConfigService->createCharacteristic(
            WIFI_SSID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        _chars.wifiSsid->setCallbacks(_pCharCallbacks);
        _chars.wifiSsid->setValue("");

        _chars.wifiPass = pConfigService->createCharacteristic(
            WIFI_PASS_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        _chars.wifiPass->setCallbacks(_pCharCallbacks);

        _chars.mqttHost = pConfigService->createCharacteristic(
            MQTT_HOST_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        _chars.mqttHost->setCallbacks(_pCharCallbacks);
        _chars.mqttHost->setValue("");

        _chars.mqttPort = pConfigService->createCharacteristic(
            MQTT_PORT_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        _chars.mqttPort->setCallbacks(_pCharCallbacks);
        _chars.mqttPort->setValue("1883");

        _chars.mqttUsername = pConfigService->createCharacteristic(
            MQTT_USERNAME_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        _chars.mqttUsername->setCallbacks(_pCharCallbacks);
        _chars.mqttUsername->setValue("");

        _chars.mqttPassword = pConfigService->createCharacteristic(
            MQTT_PASSWORD_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        _chars.mqttPassword->setCallbacks(_pCharCallbacks);

        _chars.configStatus = pConfigService->createCharacteristic(
            CONFIG_STATUS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        _chars.configStatus->setValue(Status::READY);

        _chars.deviceId = pConfigService->createCharacteristic(
            DEVICE_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        _chars.deviceId->setValue(_deviceId.empty() ? "unknown" : _deviceId);

        _chars.ipAddress = pConfigService->createCharacteristic(
            IP_ADDRESS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        _chars.ipAddress->setValue("");

        _chars.serverId = pConfigService->createCharacteristic(
            SERVER_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        _chars.serverId->setCallbacks(_pCharCallbacks);
        _chars.serverId->setValue("");

        _chars.wifiNetworks = pConfigService->createCharacteristic(
            WIFI_NETWORKS_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        _chars.wifiNetworks->setValue("[]");

        _chars.pin = pConfigService->createCharacteristic(
            PIN_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        _chars.pin->setCallbacks(_pCharCallbacks);

        pConfigService->start();

        NimBLEService* pDevInfoService = _pServer->createService("180A");
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

    common::patterns::Result<void> PairingManager::startPairing()
    {
        if (!_initialized)
        {
            auto initResult = initialize();
            if (!initResult.succeed())
            {
                return common::patterns::Result<void>::failure(initResult.error());
            }
        }

        _currentPin = generatePin();
        _pairingStartTime = millis();
        
        char msg[50];
        snprintf(msg, sizeof(msg), "[BLE] Starting pairing with PIN: %s", _currentPin.c_str());
        LOG_INFO(msg);

        _pServer = NimBLEDevice::createServer();
        
        delete _pServerCallbacks;
        _pServerCallbacks = createServerCallbacks(*this);
        _pServer->setCallbacks(_pServerCallbacks);

        if (!_pCharCallbacks)
        {
            _pCharCallbacks = createCharacteristicCallbacks(*this);
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

        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> PairingManager::stopPairing()
    {
        if (_state == PairingState::IDLE)
        {
            return common::patterns::Result<void>::success();
        }

        NimBLEDevice::getAdvertising()->stop();
        
        if (_pServer && _pServer->getConnectedCount() > 0)
        {
            _pServer->disconnect(0);
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

        if (_pServer && _pServer->getConnectedCount() > 0)
        {
            NimBLEConnInfo connInfo = _pServer->getPeerInfo(0);
            
            if (connInfo.isEncrypted())
            {
                if (_state == PairingState::ADVERTISING || _state == PairingState::AWAITING_PIN)
                {
                    _state = PairingState::AWAITING_WIFI_CONFIG;
                    LOG_INFO("[BLE] Waiting for WiFi configuration");
                    scanWifiNetworks();
                    setConfigStatus(Status::AWAITING_CONFIG);
                }

                if (_state == PairingState::AWAITING_WIFI_CONFIG && _chars.wifiSsid && _chars.wifiPass)
                {
                    std::string ssid = _chars.wifiSsid->getValue();
                    std::string pass = _chars.wifiPass->getValue();
                    
                    if (!ssid.empty() && !pass.empty())
                    {
                        LOG_INFO("[BLE] WiFi credentials received");
                        _state = PairingState::CONFIGURING_WIFI;
                        
                        processMqttConfig();

                        if (_wifiConfigCallback)
                        {
                            WifiCredentials creds{ssid, pass};
                            _wifiConfigCallback(creds);
                        }

                        _chars.wifiSsid->setValue("");
                        _chars.wifiPass->setValue("");
                        if (_chars.mqttPassword) _chars.mqttPassword->setValue("");
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
        if (success)
        {
            setConfigStatus(Status::WIFI_CONFIGURED);
            setState(PairingState::PAIRED);
            LOG_INFO("[BLE] WiFi configuration successful");
            if (_pairingCompleteCallback)
            {
                _pairingCompleteCallback(true);
            }
        }
        else
        {
            setConfigStatus(Status::WIFI_FAILED);
            LOG_INFO("[BLE] WiFi configuration failed");
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
        if (_chars.ipAddress)
        {
            _chars.ipAddress->setValue(ipAddress);
            if (_pServer && _pServer->getConnectedCount() > 0)
            {
                _chars.ipAddress->notify();
            }
            
            char msg[64];
            snprintf(msg, sizeof(msg), "[BLE] IP address set: %s", ipAddress.c_str());
            LOG_INFO(msg);
        }
    }

    void PairingManager::setDeviceId(const std::string& deviceId)
    {
        _deviceId = deviceId;
        
        if (_chars.deviceId)
        {
            _chars.deviceId->setValue(deviceId);
        }
        
        char msg[80];
        snprintf(msg, sizeof(msg), "[BLE] Device ID set: %s", deviceId.c_str());
        LOG_INFO(msg);
    }

    std::string PairingManager::getServerId() const
    {
        if (_chars.serverId)
        {
            return _chars.serverId->getValue();
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
            if (_chars.wifiNetworks)
            {
                _chars.wifiNetworks->setValue("[]");
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

        if (_chars.wifiNetworks)
        {
            _chars.wifiNetworks->setValue(json);
            LOG_INFO("[BLE] WiFi networks list updated");
        }
    }

} // namespace plant_nanny::services::bluetooth
