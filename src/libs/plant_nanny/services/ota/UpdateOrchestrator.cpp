#include "libs/plant_nanny/services/ota/UpdateOrchestrator.h"
#include "libs/common/utils/LogMacros.h"

namespace plant_nanny::services::ota
{
    UpdateOrchestrator::UpdateOrchestrator()
        : logger_(common::service::get<common::logger::Logger>()), ota_manager_(std::make_unique<ota::Manager>()), network_service_(common::service::get<network::INetworkService>()), bytes_downloaded_(0), total_bytes_(0)
    {
        LOG_IF_AVAILABLE(logger_, info, "OTA Update Orchestrator initialized");
    }

    common::patterns::Result<void> UpdateOrchestrator::update_from_url(const std::string &firmware_url)
    {
        if (!network_service_->is_connected())
        {
            LOG_IF_AVAILABLE(logger_, error, "Network not connected");
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Network not connected"));
        }

        LOG_IF_AVAILABLE(logger_, info, "Starting OTA update from URL");

        // Start OTA update
        auto start_result = ota_manager_->start_update();
        if (start_result.failed())
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to start OTA update");
            return start_result;
        }

        bytes_downloaded_ = 0;
        total_bytes_ = 0;

        // Download and write firmware
        auto download_result = network_service_->download_file(
            firmware_url,
            [this](const uint8_t *data, size_t length) -> bool
            {
                auto write_result = ota_manager_->write_chunk(data, length);
                if (write_result.failed())
                {
                    LOG_IF_AVAILABLE(logger_, error, "Failed to write OTA chunk");
                    ota_manager_->abort_update();
                    return false;
                }
                return true;
            },
            [this](size_t downloaded, size_t total)
            {
                bytes_downloaded_ = downloaded;
                total_bytes_ = total;
                if (total > 0 && downloaded % 10240 == 0) // Log every 10KB
                {
                    LOG_IF_AVAILABLE(logger_, debug, "OTA progress");
                }
            });

        if (download_result.failed())
        {
            LOG_IF_AVAILABLE(logger_, error, "Download failed");
            ota_manager_->abort_update();
            return download_result;
        }

        // Finalize OTA update
        auto finalize_result = ota_manager_->finalize_update();
        if (finalize_result.failed())
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to finalize OTA update");
            return finalize_result;
        }

        LOG_IF_AVAILABLE(logger_, info, "OTA update completed successfully");
        return common::patterns::Result<void>::success();
    }

    uint8_t UpdateOrchestrator::get_progress() const
    {
        if (total_bytes_ == 0)
        {
            return 0;
        }
        return static_cast<uint8_t>((bytes_downloaded_ * 100) / total_bytes_);
    }

} // namespace plant_nanny::services::ota
