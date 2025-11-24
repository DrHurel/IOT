#pragma once

#include "libs/common/patterns/Result.h"
#include "libs/common/logger/Logger.h"
#include "libs/common/service/Accessor.h"
#include "libs/plant_nanny/services/ota/Manager.h"
#include "libs/plant_nanny/services/network/INetworkService.h"
#include <string>
#include <memory>

namespace plant_nanny::services::ota
{
    class UpdateOrchestrator
    {
    private:
        common::service::Accessor<common::logger::Logger> logger_;
        std::unique_ptr<ota::Manager> ota_manager_;
        network::INetworkService& network_service_;
        
        size_t bytes_downloaded_;
        size_t total_bytes_;

    public:
        explicit UpdateOrchestrator(network::INetworkService& net_service);
        ~UpdateOrchestrator() = default;
        UpdateOrchestrator(const UpdateOrchestrator &) = delete;
        UpdateOrchestrator &operator=(const UpdateOrchestrator &) = delete;
        UpdateOrchestrator(UpdateOrchestrator &&) = delete;
        UpdateOrchestrator &operator=(UpdateOrchestrator &&) = delete;

        // Perform OTA update from URL
        common::patterns::Result<void> update_from_url(const std::string& firmware_url);
        
        // Get progress (0-100)
        uint8_t get_progress() const;
    };

} // namespace plant_nanny::services::ota
