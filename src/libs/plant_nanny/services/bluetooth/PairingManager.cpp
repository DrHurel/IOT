#include "libs/plant_nanny/services/bluetooth/PairingManager.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <WiFi.h>
#include "libs/common/service/Accessor.h"
#include "libs/common/logger/Logger.h"

namespace plant_nanny::services::bluetooth
{
    // Forward declarations of static variables (defined later but used in callbacks)
    static NimBLECharacteristic* pConfigStatusChar = nullptr;
    static NimBLECharacteristic* pWifiSsidChar = nullptr;

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
            snprintf(msg, sizeof(msg), "[BLE] Received write on %s: %s", uuid.c_str(), 
                (uuid == PairingManager::PIN_CHAR_UUID) ? "***" : value.c_str());
            log_info(msg);

            // Handle PIN verification
            if (uuid == PairingManager::PIN_CHAR_UUID)
            {
                if (g_pairingManager->getState() == PairingState::AWAITING_PIN)
                {
                    bool pinValid = g_pairingManager->verifyPin(value);
                    if (pinValid)
                    {
                        log_info("[BLE] PIN verified successfully");
                        g_pairingManager->setState(PairingState::AWAITING_WIFI_CONFIG);
                        if (pConfigStatusChar)
                        {
                            pConfigStatusChar->setValue("PIN_OK");
                            pConfigStatusChar->notify();
                        }
                    }
                    else
                    {
                        log_info("[BLE] PIN verification failed");
                        if (pConfigStatusChar)
                        {
                            pConfigStatusChar->setValue("PIN_INVALID");
                            pConfigStatusChar->notify();
                        }
                    }
                }
            }
            // Handle WiFi password write - this triggers WiFi configuration
            else if (uuid == PairingManager::WIFI_PASS_CHAR_UUID)
            {
                if (g_pairingManager->getState() == PairingState::AWAITING_WIFI_CONFIG)
                {
                    // Get SSID and password
                    std::string ssid = pWifiSsidChar ? pWifiSsidChar->getValue() : "";
                    std::string pass = value;
                    
                    if (!ssid.empty() && !pass.empty())
                    {
                        log_info("[BLE] WiFi credentials received via write callback");
                        g_pairingManager->handleWifiCredentials(ssid, pass);
                    }
                    else
                    {
                        log_info("[BLE] WiFi credentials incomplete");
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
            log_info("[BLE] Device connected");
            
            // Set state to awaiting PIN - user must enter PIN in the app
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
            log_info("[BLE] Device disconnected");
            // Only restart advertising if pairing/config not complete
            if (g_pairingManager)
            {
                auto state = g_pairingManager->getState();
                if (state != PairingState::PAIRED && state != PairingState::IDLE)
                {
                    // Go back to advertising if connection dropped during setup
                    g_pairingManager->setState(PairingState::ADVERTISING);
                    pServer->startAdvertising();
                }
            }
        }

        // Not using OS-level PIN anymore, but keep the callback for compatibility
        uint32_t onPassKeyDisplay() override
        {
            return 0;
        }

        void onAuthenticationComplete(NimBLEConnInfo& connInfo) override
        {
            // Not using OS-level authentication, PIN is verified via app
            log_info("[BLE] Authentication complete (app-level PIN used)");
        }
    };

    static NimBLEServer* pServer = nullptr;
    static PairingServerCallbacks* pServerCallbacks = nullptr;
    static ConfigCharacteristicCallbacks* pCharCallbacks = nullptr;
    // pWifiSsidChar is forward declared at the top
    static NimBLECharacteristic* pWifiPassChar = nullptr;
    static NimBLECharacteristic* pMqttHostChar = nullptr;
    static NimBLECharacteristic* pMqttPortChar = nullptr;
    // pConfigStatusChar is forward declared at the top
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

        // WiFi Networks characteristic - readable (JSON array of available networks)
        pWifiNetworksChar = pConfigService->createCharacteristic(
            WIFI_NETWORKS_CHAR_UUID,
            NIMBLE_PROPERTY::READ
        );
        pWifiNetworksChar->setValue("[]");

        // PIN characteristic - writable (app sends PIN for verification)
        pPinChar = pConfigService->createCharacteristic(
            PIN_CHAR_UUID,
            NIMBLE_PROPERTY::WRITE
        );
        pPinChar->setCallbacks(pCharCallbacks);

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
        // No OS-level security required - PIN verification is done in app
        NimBLEDevice::setSecurityAuth(false, false, false);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

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
                    
                    // Scan for available WiFi networks
                    scanWifiNetworks();
                    
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
                setState(PairingState::PAIRED);
                log_info("[BLE] WiFi configuration successful");
                
                // Only notify if client is still connected
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
                log_info("[BLE] WiFi configuration failed");
                
                // Only notify if client is still connected
                if (pServer && pServer->getConnectedCount() > 0)
                {
                    pConfigStatusChar->notify();
                }
            }
        }
    }

    void PairingManager::handleWifiCredentials(const std::string& ssid, const std::string& password)
    {
        log_info("[BLE] Processing WiFi credentials");
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
            
            // Only notify if client is still connected
            if (pServer && pServer->getConnectedCount() > 0)
            {
                pIpAddressChar->notify();
            }
            
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

    void PairingManager::scanWifiNetworks()
    {
        log_info("[BLE] Scanning for WiFi networks...");
        
        // Scan for networks
        int numNetworks = WiFi.scanNetworks(false, false, false, 300);
        
        if (numNetworks < 0)
        {
            log_info("[BLE] WiFi scan failed");
            if (pWifiNetworksChar)
            {
                pWifiNetworksChar->setValue("[]");
            }
            return;
        }

        char msg[48];
        snprintf(msg, sizeof(msg), "[BLE] Found %d networks", numNetworks);
        log_info(msg);

        // Build JSON array of networks (format: ["SSID1","SSID2",...])
        // BLE characteristic has limited size, so we limit to ~10 networks
        std::string json = "[";
        int count = 0;
        const int maxNetworks = 10;
        
        for (int i = 0; i < numNetworks && count < maxNetworks; i++)
        {
            String ssid = WiFi.SSID(i);
            
            // Skip empty or duplicate SSIDs
            if (ssid.length() == 0)
            {
                continue;
            }
            
            // Check for duplicates (same SSID from different APs)
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
            
            // Escape quotes in SSID
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

        // Clean up scan results
        WiFi.scanDelete();

        if (pWifiNetworksChar)
        {
            pWifiNetworksChar->setValue(json);
            log_info("[BLE] WiFi networks list updated");
        }
    }

} // namespace plant_nanny::services::bluetooth
