#pragma once

#include "libs/common/patterns/Result.h"
#include <string>
#include <cstdint>
#include <functional>

namespace plant_nanny::services::network
{
    class INetworkService
    {
    public:
        virtual ~INetworkService() = default;

        virtual void set_credentials(const std::string &ssid, const std::string &password) = 0;

        // Connection management
        virtual common::patterns::Result<void> connect() = 0;
        virtual void disconnect() = 0;
        virtual bool is_connected() const = 0;
        virtual void maintain_connection() = 0;

        // Network info
        virtual common::patterns::Result<std::string> get_ip_address() const = 0;
        virtual common::patterns::Result<int> get_rssi() const = 0;

        // HTTP utilities for OTA
        virtual common::patterns::Result<void> download_file(
            const std::string &url,
            std::function<bool(const uint8_t *, size_t)> chunk_handler,
            std::function<void(size_t, size_t)> progress_callback = nullptr) = 0;
    };

} // namespace plant_nanny::services::network
