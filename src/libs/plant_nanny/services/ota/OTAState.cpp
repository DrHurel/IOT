#include "libs/plant_nanny/services/ota/OTAState.h"

namespace plant_nanny::services::ota
{
    OTAState::OTAState()
        : update_partition_(nullptr), update_handle_(0), is_updating_(false), bytes_written_(0) {}

    bool OTAState::is_updating() const { return is_updating_; }
    size_t OTAState::bytes_written() const { return bytes_written_; }
    const esp_partition_t *OTAState::partition() const { return update_partition_; }
    esp_ota_handle_t OTAState::handle() const { return update_handle_; }

    void OTAState::begin_update(const esp_partition_t *partition, esp_ota_handle_t handle)
    {
        update_partition_ = partition;
        update_handle_ = handle;
        is_updating_ = true;
        bytes_written_ = 0;
    }

    void OTAState::add_bytes(size_t bytes) { bytes_written_ += bytes; }

    void OTAState::reset()
    {
        update_partition_ = nullptr;
        update_handle_ = 0;
        is_updating_ = false;
        bytes_written_ = 0;
    }

    common::patterns::Result<void> OTAState::validate_updating() const
    {
        if (!is_updating_)
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("OTA update not started"));
        }
        return common::patterns::Result<void>::success();
    }

    common::patterns::Result<void> OTAState::validate_not_updating() const
    {
        if (is_updating_)
        {
            return common::patterns::Result<void>::failure(
                common::patterns::Error("OTA update already in progress"));
        }
        return common::patterns::Result<void>::success();
    }

} // namespace plant_nanny::services::ota
