#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"

namespace plant_nanny::services::bluetooth
{
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

        // Global pointer for callbacks to access the manager
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
            snprintf(msg, sizeof(msg), "[BLE] Received write on %s", uuid.c_str());
            log_info(msg);
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
            log_info("[BLE] Device connected");
        }

        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
        {
            log_info("[BLE] Device disconnected");
            if (g_pairingManager && 
                (g_pairingManager->getState() == PairingState::ADVERTISING ||
                 g_pairingManager->getState() == PairingState::AWAITING_PIN ||
                 g_pairingManager->getState() == PairingState::AWAITING_WIFI_CONFIG))
            {
                pServer->startAdvertising();
            }
        }

        uint32_t onPassKeyDisplay() override
        {
            uint32_t passkey = static_cast<uint32_t>(atoi(_expectedPin.c_str()));
            char msg[40];
            snprintf(msg, sizeof(msg), "[BLE] Passkey display: %06d", passkey);
            log_info(msg);
            return passkey;
        }

        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override
        {
            if (connInfo.isEncrypted())
            {
                log_info("[BLE] Pairing successful - encrypted connection");
            }
            else
            {
                log_info("[BLE] Pairing completed without encryption");
            }
        }
    };

    static NimBLEServer* pServer = nullptr;
    static PairingServerCallbacks* pServerCallbacks = nullptr;
    static ConfigCharacteristicCallbacks* pCharCallbacks = nullptr;
    static NimBLECharacteristic* pWifiSsidChar = nullptr;
    static NimBLECharacteristic* pWifiPassChar = nullptr;
    static NimBLECharacteristic* pMqttHostChar = nullptr;
    static NimBLECharacteristic* pMqttPortChar = nullptr;
    static NimBLECharacteristic* pConfigStatusChar = nullptr;
    static NimBLECharacteristic* pDeviceIdChar = nullptr;
    static NimBLECharacteristic* pIpAddressChar = nullptr;
    static NimBLECharacteristic* pServerIdChar = nullptr;

    PairingManager::PairingManager()
        : _state(PairingState::IDLE)
        , _currentPin("")
        , _pinDisplayCallback(nullptr)
        , _pairingCompleteCallback(nullptr)
        , _wifiConfigCallback(nullptr)
        , _mqttConfigCallback(nullptr)
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
        // Create Configuration Service
        NimBLEService* pConfigService = pServer->createService(CONFIG_SERVICE_UUID);

        // WiFi SSID characteristic - writable
        pWifiSsidChar = pConfigService->createCharacteristic(
            WIFI_SSID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pWifiSsidChar->setCallbacks(pCharCallbacks);
        pWifiSsidChar->setValue("");

        // WiFi Password characteristic - writable
        pWifiPassChar = pConfigService->createCharacteristic(
            WIFI_PASS_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        pWifiPassChar->setCallbacks(pCharCallbacks);

        // MQTT Host characteristic - writable
        pMqttHostChar = pConfigService->createCharacteristic(
            MQTT_HOST_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pMqttHostChar->setCallbacks(pCharCallbacks);
        pMqttHostChar->setValue("");

        // MQTT Port characteristic - writable
        pMqttPortChar = pConfigService->createCharacteristic(
            MQTT_PORT_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pMqttPortChar->setCallbacks(pCharCallbacks);
        pMqttPortChar->setValue("1883");

        // Config Status characteristic - readable/notify
        pConfigStatusChar = pConfigService->createCharacteristic(
            CONFIG_STATUS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        pConfigStatusChar->setValue("READY");

        // Device ID characteristic - readable
        pDeviceIdChar = pConfigService->createCharacteristic(
            DEVICE_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        pDeviceIdChar->setValue("plantnanny-device");

        // IP Address characteristic - readable (set after WiFi connects)
        pIpAddressChar = pConfigService->createCharacteristic(
            IP_ADDRESS_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
        );
        pIpAddressChar->setValue("");

        // Server ID characteristic - writable (app sends server-assigned ID)
        pServerIdChar = pConfigService->createCharacteristic(
            SERVER_ID_CHAR_UUID,
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
        );
        pServerIdChar->setCallbacks(pCharCallbacks);
        pServerIdChar->setValue("");

        pConfigService->start();

        // Device Information Service
        NimBLEService* pDevInfoService = pServer->createService("180A");
        NimBLECharacteristic* pManufacturer = pDevInfoService->createCharacteristic(
            "2A29",
            NIMBLE_PROPERTY::READ
        );
        pManufacturer->setValue(DEVICE_NAME);
        pDevInfoService->start();

        log_info("[BLE] Services created");
    }

    common::patterns::Result<void> PairingManager::initialize()
    {
        if (_initialized)
        {
            return common::patterns::Result<void>::success();
        }

        NimBLEDevice::init(DEVICE_NAME);
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);

        _initialized = true;
        _state = PairingState::IDLE;

        log_info("[BLE] Pairing manager initialized");

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
        
        uint32_t passkey = static_cast<uint32_t>(atoi(_currentPin.c_str()));
        NimBLEDevice::setSecurityPasskey(passkey);
        
        char msg[50];
        snprintf(msg, sizeof(msg), "[BLE] Starting pairing with PIN: %s", _currentPin.c_str());
        log_info(msg);

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

        log_info("[BLE] Advertising started");

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

        log_info("[BLE] Pairing stopped");

        return common::patterns::Result<void>::success();
    }

    void PairingManager::update()
    {
        if (_state == PairingState::IDLE || _state == PairingState::PAIRED)
        {
            return;
        }

        // Check for timeout
        if (_pairingStartTime > 0 && (millis() - _pairingStartTime) > PAIRING_TIMEOUT_MS)
        {
            log_info("[BLE] Pairing timeout");
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
                    log_info("[BLE] Waiting for WiFi configuration");
                    
                    if (pConfigStatusChar)
                    {
                        pConfigStatusChar->setValue("AWAITING_CONFIG");
                        pConfigStatusChar->notify();
                    }
                }

                // Check if WiFi credentials have been written
                if (_state == PairingState::AWAITING_WIFI_CONFIG && pWifiSsidChar && pWifiPassChar)
                {
                    std::string ssid = pWifiSsidChar->getValue();
                    std::string pass = pWifiPassChar->getValue();
                    
                    if (!ssid.empty() && !pass.empty())
                    {
                        log_info("[BLE] WiFi credentials received");
                        _state = PairingState::CONFIGURING_WIFI;
                        
                        std::string mqttHost = pMqttHostChar ? pMqttHostChar->getValue() : "";
                        std::string mqttPortStr = pMqttPortChar ? pMqttPortChar->getValue() : "1883";
                        uint16_t mqttPort = static_cast<uint16_t>(atoi(mqttPortStr.c_str()));
                        if (mqttPort == 0) mqttPort = 1883;

                        if (_wifiConfigCallback)
                        {
                            WifiCredentials creds{ssid, pass};
                            _wifiConfigCallback(creds);
                        }

                        if (!mqttHost.empty() && _mqttConfigCallback)
                        {
                            MqttConfig config{mqttHost, mqttPort, "", ""};
                            _mqttConfigCallback(config);
                        }

                        // Clear for security
                        pWifiSsidChar->setValue("");
                        pWifiPassChar->setValue("");
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
                _state = PairingState::PAIRED;
                log_info("[BLE] WiFi configuration successful");
                
                if (_pairingCompleteCallback)
                {
                    _pairingCompleteCallback(true);
                }
            }
            else
            {
                pConfigStatusChar->setValue("WIFI_FAILED");
                log_info("[BLE] WiFi configuration failed");
            }
            pConfigStatusChar->notify();
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

    void PairingManager::onMqttConfigReceived(const std::string& host, uint16_t port)
    {
        if (_mqttConfigCallback)
        {
            MqttConfig config{host, port, "", ""};
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

        log_info("[BLE] All bonds cleared");

        return common::patterns::Result<void>::success();
    }

    void PairingManager::setIpAddress(const std::string& ipAddress)
    {
        if (pIpAddressChar)
        {
            pIpAddressChar->setValue(ipAddress);
            pIpAddressChar->notify();
            
            char msg[64];
            snprintf(msg, sizeof(msg), "[BLE] IP address set: %s", ipAddress.c_str());
            log_info(msg);
        }
    }

    std::string PairingManager::getServerId() const
    {
        if (pServerIdChar)
        {
            return pServerIdChar->getValue();
        }
        return "";
    }

} // namespace plant_nanny::services::bluetooth
