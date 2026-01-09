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
    }

    // Static callback class for NimBLE security
    class PairingSecurityCallbacks : public NimBLEServerCallbacks
    {
    private:
        PairingManager* _manager;
        std::string _expectedPin;

    public:
        PairingSecurityCallbacks(PairingManager* manager, const std::string& pin)
            : _manager(manager), _expectedPin(pin) {}

        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override
        {
            log_info("[BLE] Device connected");
        }

        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override
        {
            log_info("[BLE] Device disconnected");
            // Restart advertising if we're still in pairing mode
            if (_manager->getState() == PairingState::ADVERTISING || 
                _manager->getState() == PairingState::AWAITING_PIN)
            {
                pServer->startAdvertising();
            }
        }

        uint32_t onPassKeyDisplay() override
        {
            // Convert PIN string to uint32_t
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
    static PairingSecurityCallbacks* pSecurityCallbacks = nullptr;

    PairingManager::PairingManager()
        : _state(PairingState::IDLE)
        , _currentPin("")
        , _pinDisplayCallback(nullptr)
        , _pairingCompleteCallback(nullptr)
        , _initialized(false)
        , _pairingStartTime(0)
    {
    }

    PairingManager::~PairingManager()
    {
        if (_initialized)
        {
            stopPairing();
            NimBLEDevice::deinit(true);
        }
        if (pSecurityCallbacks)
        {
            delete pSecurityCallbacks;
            pSecurityCallbacks = nullptr;
        }
    }

    std::string PairingManager::generatePin()
    {
        // Generate a random 6-digit PIN (100000-999999) - BLE standard
        uint32_t pin = 100000 + (esp_random() % 900000);
        char pinStr[7];
        snprintf(pinStr, sizeof(pinStr), "%06d", pin);
        return std::string(pinStr);
    }

    common::patterns::Result<void> PairingManager::initialize()
    {
        if (_initialized)
        {
            return common::patterns::Result<void>::success();
        }

        // Initialize NimBLE
        NimBLEDevice::init(DEVICE_NAME);
        
        // Set security - require passkey entry (static passkey mode)
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);  // We display PIN, phone enters it

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

        // Generate new PIN
        _currentPin = generatePin();
        _pairingStartTime = millis();
        
        // Set the passkey that NimBLE will use for authentication
        uint32_t passkey = static_cast<uint32_t>(atoi(_currentPin.c_str()));
        NimBLEDevice::setSecurityPasskey(passkey);
        
        char msg[50];
        snprintf(msg, sizeof(msg), "[BLE] Starting pairing with PIN: %s", _currentPin.c_str());
        log_info(msg);

        // Create/get server
        pServer = NimBLEDevice::createServer();
        
        // Set up security callbacks with our PIN
        if (pSecurityCallbacks)
        {
            delete pSecurityCallbacks;
        }
        pSecurityCallbacks = new PairingSecurityCallbacks(this, _currentPin);
        pServer->setCallbacks(pSecurityCallbacks);

        // Create a simple service for pairing
        NimBLEService* pService = pServer->createService("180A");  // Device Information Service
        NimBLECharacteristic* pChar = pService->createCharacteristic(
            "2A29",  // Manufacturer Name
            NIMBLE_PROPERTY::READ
        );
        pChar->setValue(DEVICE_NAME);
        pService->start();

        // Start advertising with device name visible
        NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->reset();
        pAdvertising->setName(DEVICE_NAME);
        pAdvertising->addServiceUUID("180A");
        pAdvertising->enableScanResponse(true);
        pAdvertising->start();

        _state = PairingState::ADVERTISING;

        // Notify UI to display PIN
        if (_pinDisplayCallback)
        {
            _pinDisplayCallback(_currentPin);
        }

        {
            char msg[50];
            snprintf(msg, sizeof(msg), "[BLE] Advertising started. PIN: %s", _currentPin.c_str());
            log_info(msg);
        }

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

        // Check if connected and bonded
        if (pServer && pServer->getConnectedCount() > 0)
        {
            // Check if the connection is encrypted (successful pairing)
            NimBLEConnInfo connInfo = pServer->getPeerInfo(0);
            if (connInfo.isEncrypted() && connInfo.isBonded())
            {
                _state = PairingState::PAIRED;
                NimBLEDevice::getAdvertising()->stop();
                
                log_info("[BLE] Pairing completed successfully");
                
                if (_pairingCompleteCallback)
                {
                    _pairingCompleteCallback(true);
                }
            }
            else if (_state == PairingState::ADVERTISING)
            {
                _state = PairingState::AWAITING_PIN;
            }
        }
    }

    common::patterns::Result<void> PairingManager::unpair()
    {
        stopPairing();
        
        // Clear all bonded devices
        int numBonds = NimBLEDevice::getNumBonds();
        for (int i = 0; i < numBonds; i++)
        {
            NimBLEDevice::deleteBond(NimBLEDevice::getBondedAddress(0));
        }

        _state = PairingState::IDLE;

        log_info("[BLE] All bonds cleared");

        return common::patterns::Result<void>::success();
    }

} // namespace plant_nanny::services::bluetooth
