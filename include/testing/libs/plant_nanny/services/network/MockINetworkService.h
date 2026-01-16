#pragma once

#include "libs/plant_nanny/services/network/INetworkService.h"
#include <string>
#include <cstdint>
#include <functional>
#include <fstream>
#include <cstdlib>

#ifndef NATIVE_TEST
#include <Arduino.h>
#include <WiFi.h>
#endif

namespace testing::mocks
{
    struct MockNetworkConfig
    {
        bool connect_should_succeed = true;
        bool download_should_succeed = true;
        bool use_real_connection = false;
        std::string mock_ip = "192.168.1.100";
        int mock_rssi = -50;
    };

    class MockNetworkService : public plant_nanny::services::network::INetworkService
    {
    private:
        std::string ssid_;
        std::string password_;
        bool connected_{false};
        MockNetworkConfig config_;

    public:
        MockNetworkService() = default;
        explicit MockNetworkService(const MockNetworkConfig &config) : config_(config) {}
        ~MockNetworkService() override = default;

        common::patterns::Result<void> load_credentials_from_env(const std::string &env_path = ".env")
        {
            std::ifstream env_file(env_path);
            if (!env_file.is_open())
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("Failed to open .env file: " + env_path));
            }

            std::string line;
            while (std::getline(env_file, line))
            {
                if (line.empty() || line[0] == '#')
                {
                    continue;
                }

                size_t pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    key.erase(0, key.find_first_not_of(" \t\r\n"));
                    key.erase(key.find_last_not_of(" \t\r\n") + 1);
                    value.erase(0, value.find_first_not_of(" \t\r\n"));
                    value.erase(value.find_last_not_of(" \t\r\n") + 1);

                    if (key == "WIFI_SSID")
                    {
                        ssid_ = value;
                    }
                    else if (key == "WIFI_PASSWORD")
                    {
                        password_ = value;
                    }
                }
            }

            if (ssid_.empty())
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("WIFI_SSID not found or empty in .env file"));
            }

            if (password_.empty())
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("WIFI_PASSWORD not found or empty in .env file"));
            }

            return common::patterns::Result<void>::success();
        }

        common::patterns::Result<std::string> get_stored_ssid() const
        {
            if (ssid_.empty())
            {
                return common::patterns::Result<std::string>::failure(
                    common::patterns::Error("SSID not set"));
            }
            return common::patterns::Result<std::string>::success(ssid_);
        }

        common::patterns::Result<std::string> get_stored_password() const
        {
            if (password_.empty())
            {
                return common::patterns::Result<std::string>::failure(
                    common::patterns::Error("Password not set"));
            }
            return common::patterns::Result<std::string>::success(password_);
        }

        const MockNetworkConfig &get_config() const { return config_; }

        void test_set_connected(bool connected) { connected_ = connected; }
        void test_update_config(const MockNetworkConfig &config) { config_ = config; }

        void set_credentials(const std::string &ssid, const std::string &password) override
        {
            ssid_ = ssid;
            password_ = password;
        }

        common::patterns::Result<void> connect() override
        {
            if (config_.use_real_connection)
            {
#ifndef NATIVE_TEST
                if (ssid_.empty())
                {
                    return common::patterns::Result<void>::failure(
                        common::patterns::Error("WiFi credentials not set"));
                }

                WiFi.mode(WIFI_STA);
                WiFi.begin(ssid_.c_str(), password_.c_str());

                uint32_t start_time = millis();
                while (WiFi.status() != WL_CONNECTED && (millis() - start_time) < 10000)
                {
                    delay(500);
                }

                if (WiFi.status() == WL_CONNECTED)
                {
                    connected_ = true;
                    config_.mock_ip = WiFi.localIP().toString().c_str();
                    config_.mock_rssi = WiFi.RSSI();
                    return common::patterns::Result<void>::success();
                }
                else
                {
                    connected_ = false;
                    return common::patterns::Result<void>::failure(
                        common::patterns::Error("Real WiFi connection failed"));
                }
#else
                if (ssid_.empty() || password_.empty())
                {
                    return common::patterns::Result<void>::failure(
                        common::patterns::Error("Real connection mode: WiFi credentials not set"));
                }

                connected_ = true;
                return common::patterns::Result<void>::success();
#endif
            }

            if (config_.connect_should_succeed)
            {
                connected_ = true;
                return common::patterns::Result<void>::success();
            }
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Mock connection failed"));
        }

        void disconnect() override
        {
            connected_ = false;
        }

        bool is_connected() const override
        {
            return connected_;
        }

        void maintain_connection() override
        {
        }

        common::patterns::Result<std::string> get_ip_address() const override
        {
            if (!connected_)
            {
                return common::patterns::Result<std::string>::failure(
                    common::patterns::Error("Not connected"));
            }
            return common::patterns::Result<std::string>::success(config_.mock_ip);
        }

        common::patterns::Result<int> get_rssi() const override
        {
            if (!connected_)
            {
                return common::patterns::Result<int>::failure(
                    common::patterns::Error("Not connected"));
            }
            return common::patterns::Result<int>::success(config_.mock_rssi);
        }

        common::patterns::Result<void> download_file(
            const std::string &url,
            std::function<bool(const uint8_t *, size_t)> chunk_handler,
            std::function<void(size_t, size_t)> progress_callback = nullptr) override
        {
            if (!connected_)
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("Not connected"));
            }

            if (!config_.download_should_succeed)
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("Mock download failed"));
            }

            uint8_t fake_data[] = {0x01, 0x02, 0x03, 0x04};
            if (!chunk_handler(fake_data, sizeof(fake_data)))
            {
                return common::patterns::Result<void>::failure(
                    common::patterns::Error("Chunk handler failed"));
            }

            if (progress_callback)
            {
                progress_callback(sizeof(fake_data), sizeof(fake_data));
            }

            return common::patterns::Result<void>::success();
        }
    };

} // namespace testing::mocks
