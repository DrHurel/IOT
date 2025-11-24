#pragma once

#include "libs/common/patterns/Result.h"
#include "esp_ota_ops.h"
#include <cstddef>

namespace plant_nanny::services::ota
{
    class OTAState
    {
    private:
        const esp_partition_t* update_partition_;
        esp_ota_handle_t update_handle_;
        bool is_updating_;
        size_t bytes_written_;

    public:
        OTAState();

        // State queries
        bool is_updating() const;
        size_t bytes_written() const;
        const esp_partition_t* partition() const;
        esp_ota_handle_t handle() const;

        // State transitions
        void begin_update(const esp_partition_t* partition, esp_ota_handle_t handle);
        void add_bytes(size_t bytes);
        void reset();

        // Validation
        common::patterns::Result<void> validate_updating() const;
        common::patterns::Result<void> validate_not_updating() const;
    };

} // namespace plant_nanny::services::ota
