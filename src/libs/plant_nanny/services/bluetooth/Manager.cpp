#include "libs/plant_nanny/services/bluetooth/Manager.h"
#include "libs/common/utils/LogMacros.h"
#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <NimBLE2904.h>

namespace plant_nanny::services::bluetooth
{
    // Static members for BLE
    static NimBLEServer *pServer = nullptr;
    static NimBLECharacteristic *pTxCharacteristic = nullptr;
    static NimBLECharacteristic *pRxCharacteristic = nullptr;
    static bool deviceConnected = false;
    static bool oldDeviceConnected = false;
    static std::string receivedData;
    static bool dataAvailable = false;

    // Server callbacks
    class ServerCallbacks : public NimBLEServerCallbacks
    {
        void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
        {
            deviceConnected = true;
            NimBLEDevice::startAdvertising();
        }

        void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
        {
            deviceConnected = false;
        }
    };

    // Characteristic callbacks for receiving data
    class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
    {
        void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
        {
            receivedData = pCharacteristic->getValue();
            dataAvailable = true;
        }
    };

    Manager::Manager()
        : logger_(common::service::get<common::logger::Logger>())
    {
        LOG_IF_AVAILABLE(logger_, info, "Bluetooth Manager initialized");
    }

    common::patterns::Result<void> Manager::open_channel()
    {
        try
        {
            LOG_IF_AVAILABLE(logger_, info, "Initializing Bluetooth...");

            // Initialize NimBLE
            NimBLEDevice::init(DEVICE_NAME);

            // Create BLE Server
            pServer = NimBLEDevice::createServer();
            pServer->setCallbacks(new ServerCallbacks());

            // Create BLE Service
            NimBLEService *pService = pServer->createService(SERVICE_UUID);

            // Create TX Characteristic (for sending data to client)
            pTxCharacteristic = pService->createCharacteristic(
                CHAR_UUID_TX,
                NIMBLE_PROPERTY::NOTIFY);

            // Create RX Characteristic (for receiving data from client)
            pRxCharacteristic = pService->createCharacteristic(
                CHAR_UUID_RX,
                NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
            pRxCharacteristic->setCallbacks(new CharacteristicCallbacks());

            // Start the service
            pService->start();

            // Start advertising
            NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
            pAdvertising->addServiceUUID(SERVICE_UUID);
            NimBLEDevice::startAdvertising();

            LOG_IF_AVAILABLE(logger_, info, "Bluetooth channel opened successfully");
            return common::patterns::Result<void>::success();
        }
        catch (const std::exception &e)
        {
            LOG_IF_AVAILABLE(logger_, error, ("Bluetooth initialization failed: " + std::string(e.what())).c_str());
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Failed to open Bluetooth channel: " + std::string(e.what())));
        }
    }

    common::patterns::Result<void> Manager::close_channel()
    {
        try
        {
            LOG_IF_AVAILABLE(logger_, info, "Closing Bluetooth channel...");

            if (pServer != nullptr)
            {
                NimBLEDevice::stopAdvertising();
                NimBLEDevice::deinit(true);
                pServer = nullptr;
                pTxCharacteristic = nullptr;
                pRxCharacteristic = nullptr;
                deviceConnected = false;
                oldDeviceConnected = false;
            }

            LOG_IF_AVAILABLE(logger_, info, "Bluetooth channel closed successfully");
            return common::patterns::Result<void>::success();
        }
        catch (const std::exception &e)
        {
            LOG_IF_AVAILABLE(logger_, error, ("Failed to close Bluetooth channel: " + std::string(e.what())).c_str());
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Failed to close Bluetooth channel: " + std::string(e.what())));
        }
    }

    bool Manager::is_channel_open() const
    {
        return pServer != nullptr;
    }

    common::patterns::Result<int> Manager::send_data(const std::string &data)
    {
        if (!is_channel_open())
        {
            LOG_IF_AVAILABLE(logger_, error, "Bluetooth channel is not open");
            return common::patterns::Result<int>::failure(
                common::patterns::Error("Bluetooth channel is not open"));
        }

        if (!deviceConnected)
        {
            LOG_IF_AVAILABLE(logger_, warning, "No device connected");
            return common::patterns::Result<int>::failure(
                common::patterns::Error("No device connected"));
        }

        if (pTxCharacteristic == nullptr)
        {
            LOG_IF_AVAILABLE(logger_, error, "TX characteristic not initialized");
            return common::patterns::Result<int>::failure(
                common::patterns::Error("TX characteristic not initialized"));
        }

        try
        {
            pTxCharacteristic->setValue(data);
            pTxCharacteristic->notify();

            LOG_IF_AVAILABLE(logger_, debug, ("Sent " + std::to_string(data.length()) + " bytes via Bluetooth").c_str());
            return common::patterns::Result<int>::success(static_cast<int>(data.length()));
        }
        catch (const std::exception &e)
        {
            LOG_IF_AVAILABLE(logger_, error, ("Failed to send data: " + std::string(e.what())).c_str());
            return common::patterns::Result<int>::failure(
                common::patterns::Error("Failed to send data: " + std::string(e.what())));
        }
    }

    common::patterns::Result<std::string_view> Manager::receive_data(size_t max_length, bool require_credentials)
    {
        if (!is_channel_open())
        {
            LOG_IF_AVAILABLE(logger_, error, "Bluetooth channel is not open");
            return common::patterns::Result<std::string_view>::failure(
                common::patterns::Error("Bluetooth channel is not open"));
        }

        if (!deviceConnected)
        {
            LOG_IF_AVAILABLE(logger_, warning, "No device connected");
            return common::patterns::Result<std::string_view>::failure(
                common::patterns::Error("No device connected"));
        }

        if (!dataAvailable)
        {
            return common::patterns::Result<std::string_view>::failure(
                common::patterns::Error("No data available"));
        }

        // Reset the data available flag
        dataAvailable = false;

        // Truncate if necessary
        if (receivedData.length() > max_length)
        {
            receivedData = receivedData.substr(0, max_length);
        }

        LOG_IF_AVAILABLE(logger_, debug, ("Received " + std::to_string(receivedData.length()) + " bytes via Bluetooth").c_str());

        return common::patterns::Result<std::string_view>::success(
            std::string_view(receivedData.c_str(), receivedData.length()));
    }

} // namespace plant_nanny::services::bluetooth
