#pragma once

#include "libs/plant_nanny/services/network/INetworkService.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include <string>
#include <cstdint>

#include <WiFi.h>
#include <HTTPClient.h>

namespace plant_nanny::services::network
{
    class Manager : public INetworkService
    {
    private:
        common::service::Accessor<common::logger::Logger> logger_;
        std::string ssid_;
        std::string password_;
        uint32_t last_connection_attempt_;
        static constexpr uint32_t RECONNECT_INTERVAL_MS = 30000;

        bool attempt_connection();

    public:
        Manager();
        ~Manager() override = default;
        Manager(const Manager &) = delete;
        Manager &operator=(const Manager &) = delete;
        Manager(Manager &&) = delete;
        Manager &operator=(Manager &&) = delete;

        void set_credentials(const std::string &ssid, const std::string &password) override;
        common::patterns::Result<void> connect() override;
        void disconnect() override;
        bool is_connected() const override;
        void maintain_connection() override;
        common::patterns::Result<std::string> get_ip_address() const override;
        common::patterns::Result<int> get_rssi() const override;
        common::patterns::Result<void> download_file(
            const std::string &url,
            std::function<bool(const uint8_t *, size_t)> chunk_handler,
            std::function<void(size_t, size_t)> progress_callback = nullptr) override;
    };

} // namespace plant_nanny::services::network