#include "libs/plant_nanny/services/ota/Manager.h"
#include "libs/plant_nanny/services/ota/OTAState.h"
#include "libs/plant_nanny/services/ota/PartitionInfo.h"
#include "libs/plant_nanny/services/ota/AppInfo.h"
#include "libs/common/utils/EspError.h"
#include "libs/common/utils/LogMacros.h"

#include "esp_ota_ops.h"
#include "esp_system.h"

namespace plant_nanny::services::ota
{
    using common::utils::EspError;

    static OTAState s_ota_state;

    Manager::Manager() : logger_(common::service::get<common::logger::Logger>())
    {
        LOG_IF_AVAILABLE(logger_, info, "OTA Manager initialized");
    }

    Manager::~Manager()
    {
        if (s_ota_state.is_updating())
        {
            LOG_IF_AVAILABLE(logger_, warning, "OTA Manager destroyed while update in progress");
            esp_ota_abort(s_ota_state.handle());
            s_ota_state.reset();
        }
    }

    common::patterns::Result<void> Manager::start_update()
    {
        auto validation = s_ota_state.validate_not_updating();
        if (validation.failed())
            return validation;

        const esp_partition_t *configured = PartitionInfo::get_boot_partition();
        const esp_partition_t *running = PartitionInfo::get_running_partition();

        if (!PartitionInfo::partitions_match(configured, running))
        {
            LOG_IF_AVAILABLE(logger_, warning, "Configured OTA boot partition mismatch with running partition");
            LOG_IF_AVAILABLE(logger_, warning, "(This can happen if OTA boot data or preferred boot image become corrupted)");
        }

        auto partition_result = PartitionInfo::get_update_partition();
        if (partition_result.failed())
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to find OTA partition");
            return common::patterns::Result<void>::failure(partition_result.error());
        }

        const esp_partition_t *update_partition = partition_result.value();
        LOG_IF_AVAILABLE(logger_, info, "Writing to OTA partition");

        esp_ota_handle_t update_handle;
        esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
        if (!EspError::is_ok(err))
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to begin OTA update");
            return EspError::to_result(err, "Failed to begin OTA update");
        }

        s_ota_state.begin_update(update_partition, update_handle);
        LOG_IF_AVAILABLE(logger_, info, "OTA update started successfully");
        return common::patterns::Result<void>::success();
    }

    bool Manager::is_updating() const { return s_ota_state.is_updating(); }

    common::patterns::Result<void> Manager::write_chunk(const uint8_t *data, size_t length)
    {
        auto validation = s_ota_state.validate_updating();
        if (validation.failed())
            return validation;

        if (data == nullptr || length == 0)
            return common::patterns::Result<void>::failure(common::patterns::Error("Invalid data or length"));

        esp_err_t err = esp_ota_write(s_ota_state.handle(), data, length);
        if (!EspError::is_ok(err))
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to write OTA data");
            return EspError::to_result(err, "Failed to write OTA data");
        }

        s_ota_state.add_bytes(length);
        LOG_IF_AVAILABLE(logger_, debug, "Written OTA chunk");
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<size_t> Manager::get_bytes_written() const
    {
        auto validation = s_ota_state.validate_updating();
        if (validation.failed())
            return common::patterns::Result<size_t>::failure(validation.error());
        return common::patterns::Result<size_t>::success(s_ota_state.bytes_written());
    }

    common::patterns::Result<void> Manager::finalize_update()
    {
        auto validation = s_ota_state.validate_updating();
        if (validation.failed())
            return validation;

        esp_err_t err = esp_ota_end(s_ota_state.handle());
        if (!EspError::is_ok(err))
        {
            LOG_IF_AVAILABLE(logger_, error, err == ESP_ERR_OTA_VALIDATE_FAILED ? "Image validation failed, image is corrupted" : "Failed to end OTA update");
            s_ota_state.reset();
            return err == ESP_ERR_OTA_VALIDATE_FAILED ? common::patterns::Result<void>::failure(common::patterns::Error("OTA image validation failed - image is corrupted")) : EspError::to_result(err, "Failed to end OTA update");
        }

        err = esp_ota_set_boot_partition(s_ota_state.partition());
        if (!EspError::is_ok(err))
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to set boot partition");
            s_ota_state.reset();
            return EspError::to_result(err, "Failed to set boot partition");
        }

        LOG_IF_AVAILABLE(logger_, info, "OTA update finalized successfully");
        s_ota_state.reset();
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> Manager::abort_update()
    {
        auto validation = s_ota_state.validate_updating();
        if (validation.failed())
            return validation;

        esp_err_t err = esp_ota_abort(s_ota_state.handle());
        if (!EspError::is_ok(err))
        {
            LOG_IF_AVAILABLE(logger_, error, "Failed to abort OTA update");
            s_ota_state.reset();
            return EspError::to_result(err, "Failed to abort OTA update");
        }

        LOG_IF_AVAILABLE(logger_, info, "OTA update aborted");
        s_ota_state.reset();
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<std::string> Manager::get_running_app_version() const { return AppInfo::get_version(); }
    common::patterns::Result<std::string> Manager::get_running_app_name() const { return AppInfo::get_name(); }

    common::patterns::Result<void> Manager::reboot()
    {
        LOG_IF_AVAILABLE(logger_, info, "Rebooting system...");
        esp_restart();
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> Manager::rollback()
    {
        const esp_partition_t *running = PartitionInfo::get_running_partition();
        esp_ota_img_states_t ota_state;

        esp_err_t err = esp_ota_get_state_partition(running, &ota_state);
        if (!EspError::is_ok(err))
            return EspError::to_result(err, "Failed to get OTA state");

        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY)
        {
            err = esp_ota_mark_app_invalid_rollback_and_reboot();
            if (!EspError::is_ok(err))
                return EspError::to_result(err, "Failed to rollback");
        }
        else if (ota_state == ESP_OTA_IMG_VALID)
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("Current image is already marked as valid, cannot rollback"));
        }
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> Manager::mark_valid()
    {
        const esp_partition_t *running = PartitionInfo::get_running_partition();
        esp_ota_img_states_t ota_state;

        esp_err_t err = esp_ota_get_state_partition(running, &ota_state);
        if (!EspError::is_ok(err))
            return EspError::to_result(err, "Failed to get OTA state");

        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY)
        {
            err = esp_ota_mark_app_valid_cancel_rollback();
            if (!EspError::is_ok(err))
            {
                LOG_IF_AVAILABLE(logger_, error, "Failed to mark app as valid");
                return EspError::to_result(err, "Failed to mark app as valid");
            }
            LOG_IF_AVAILABLE(logger_, info, "App marked as valid");
        }
        else if (ota_state == ESP_OTA_IMG_VALID)
        {
            LOG_IF_AVAILABLE(logger_, info, "App already marked as valid");
        }
        return common::patterns::Result<void>::success();
    }

} // namespace plant_nanny::services::ota