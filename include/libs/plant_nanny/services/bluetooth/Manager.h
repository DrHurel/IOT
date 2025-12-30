#pragma once

#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include "libs/common/patterns/Result.h"
#include <string_view>
#include <string>
#include <cstdint>

namespace plant_nanny::services::bluetooth
{
    /**
     * @brief Bluetooth Low Energy (BLE) Manager for plant monitoring device
     *
     * This class manages BLE communication using NimBLE stack.
     * It provides methods to:
     * - Open/close BLE communication channel
     * - Send data to connected clients via notifications
     * - Receive data from connected clients via write characteristics
     *
     * Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
     * TX Characteristic (Notify): 6e400003-b5a3-f393-e0a9-e50e24dcca9e
     * RX Characteristic (Write): beb5483e-36e1-4688-b7f5-ea07361b26a8
     */
    class Manager
    {
    public:
        // BLE Service and Characteristic UUIDs
        static constexpr const char *SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
        static constexpr const char *CHAR_UUID_RX = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
        static constexpr const char *CHAR_UUID_TX = "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
        static constexpr const char *DEVICE_NAME = "PlantNanny";

    private:
        common::service::Accessor<common::logger::Logger> logger_;

    public:
        Manager();
#ifdef NATIVE_TEST
        virtual ~Manager() = default;
#else
        virtual ~Manager()
        {
            if (is_channel_open())
            {
                auto res = close_channel();
                if (res.succeed())
                {
                    logger_->info("Bluetooth channel closed successfully in destructor.");
                }
                else
                {
                    logger_->error(("Failed to close Bluetooth channel in destructor: " + res.error().message()).c_str());
                }
            }
        }
#endif
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;
        Manager(Manager &&) = delete;
        Manager &operator=(Manager &&) = delete;

        /**
         * @brief Initialize and open the BLE channel
         * @return Result<void> Success or error with message
         */
        virtual common::patterns::Result<void> open_channel();

        /**
         * @brief Close the BLE channel and cleanup resources
         * @return Result<void> Success or error with message
         */
        virtual common::patterns::Result<void> close_channel();

        /**
         * @brief Check if the BLE channel is currently open
         * @return true if channel is open, false otherwise
         */
        virtual bool is_channel_open() const;

        /**
         * @brief Send data to connected BLE client
         * @param data The data string to send
         * @return Result<int> Number of bytes sent or error
         */
        virtual common::patterns::Result<int> send_data(const std::string &data);

        /**
         * @brief Receive data from connected BLE client
         * @param max_length Maximum length of data to receive
         * @param require_credentials Whether to require authentication (not yet implemented)
         * @return Result<std::string_view> Received data or error
         */
        virtual common::patterns::Result<std::string_view> receive_data(size_t max_length, bool require_credentials = false);
    };

#ifdef NATIVE_TEST
    // Inline stub implementations for native tests
    // The mock (MockBluetoothService) will override these
    inline Manager::Manager()
    {
        // Don't initialize logger in native tests
        // Accessor default constructor would try to access DefaultRegistry which may not be initialized
    }

    inline common::patterns::Result<void> Manager::open_channel()
    {
        return common::patterns::Result<void>::failure(
            common::patterns::Error("Bluetooth not available in native tests"));
    }

    inline common::patterns::Result<void> Manager::close_channel()
    {
        return common::patterns::Result<void>::success();
    }

    inline bool Manager::is_channel_open() const
    {
        return false;
    }

    inline common::patterns::Result<int> Manager::send_data(const std::string &data)
    {
        return common::patterns::Result<int>::failure(
            common::patterns::Error("Bluetooth not available in native tests"));
    }

    inline common::patterns::Result<std::string_view> Manager::receive_data(size_t max_length, bool require_credentials)
    {
        return common::patterns::Result<std::string_view>::failure(
            common::patterns::Error("Bluetooth not available in native tests"));
    }
#endif // NATIVE_TEST

} // namespace plant_nanny::services::bluetooth