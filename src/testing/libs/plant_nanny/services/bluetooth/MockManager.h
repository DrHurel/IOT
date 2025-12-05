#pragma once

#include "libs/plant_nanny/services/bluetooth/Manager.h"
#include <string>
#include <cstdint>

namespace testing::mocks
{
    struct MockBluetoothConfig
    {
        bool channel_open_should_succeed = true;
        bool device_connected = true;
        bool has_data_available = false;
        std::string mock_received_data = "Mock data";
    };

    class MockBluetoothService : public plant_nanny::services::bluetooth::Manager
    {
    private:
        bool channel_open_{false};
        MockBluetoothConfig config_;
        std::string received_data_buffer_;

    public:
        MockBluetoothService() = default;
        explicit MockBluetoothService(const MockBluetoothConfig &config) : config_(config) {}
        ~MockBluetoothService() override = default;

        common::patterns::Result<void> open_channel() override
        {
            if (config_.channel_open_should_succeed)
            {
                channel_open_ = true;
                return common::patterns::Result<void>::success();
            }
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Mock: Failed to open Bluetooth channel"));
        }

        common::patterns::Result<void> close_channel() override
        {
            channel_open_ = false;
            return common::patterns::Result<void>::success();
        }

        bool is_channel_open() const override
        {
            return channel_open_;
        }

        common::patterns::Result<int> send_data(const std::string &data) override
        {
            if (!channel_open_)
            {
                return common::patterns::Result<int>::failure(
                    common::patterns::Error("Mock: Bluetooth channel is not open"));
            }

            if (!config_.device_connected)
            {
                return common::patterns::Result<int>::failure(
                    common::patterns::Error("Mock: No device connected"));
            }

            return common::patterns::Result<int>::success(static_cast<int>(data.length()));
        }

        common::patterns::Result<std::string_view> receive_data(size_t max_length, bool require_credentials = false) override
        {
            if (!channel_open_)
            {
                return common::patterns::Result<std::string_view>::failure(
                    common::patterns::Error("Mock: Bluetooth channel is not open"));
            }

            if (!config_.device_connected)
            {
                return common::patterns::Result<std::string_view>::failure(
                    common::patterns::Error("Mock: No device connected"));
            }

            if (!config_.has_data_available)
            {
                return common::patterns::Result<std::string_view>::failure(
                    common::patterns::Error("Mock: No data available"));
            }

            received_data_buffer_ = config_.mock_received_data;
            if (received_data_buffer_.length() > max_length)
            {
                received_data_buffer_ = received_data_buffer_.substr(0, max_length);
            }

            return common::patterns::Result<std::string_view>::success(
                std::string_view(received_data_buffer_));
        }

        // Test helper methods
        void set_device_connected(bool connected)
        {
            config_.device_connected = connected;
        }

        void set_data_available(bool available)
        {
            config_.has_data_available = available;
        }

        void set_mock_data(const std::string &data)
        {
            config_.mock_received_data = data;
        }
    };

} // namespace testing::mocks
