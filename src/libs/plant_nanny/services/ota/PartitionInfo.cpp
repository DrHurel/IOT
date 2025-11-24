#include "libs/plant_nanny/services/ota/PartitionInfo.h"
#include "esp_ota_ops.h"

namespace plant_nanny::services::ota
{
    const esp_partition_t* PartitionInfo::get_boot_partition()
    {
        return esp_ota_get_boot_partition();
    }

    const esp_partition_t* PartitionInfo::get_running_partition()
    {
        return esp_ota_get_running_partition();
    }

    const esp_partition_t* PartitionInfo::get_next_update_partition()
    {
        return esp_ota_get_next_update_partition(nullptr);
    }

    bool PartitionInfo::partitions_match(const esp_partition_t* p1, const esp_partition_t* p2)
    {
        return p1 == p2;
    }

    common::patterns::Result<const esp_partition_t*> PartitionInfo::get_update_partition()
    {
        const esp_partition_t* partition = get_next_update_partition();
        if (partition == nullptr)
        {
            return common::patterns::Result<const esp_partition_t*>::failure(
                common::patterns::Error("Failed to find OTA partition")
            );
        }
        return common::patterns::Result<const esp_partition_t*>::success(partition);
    }

} // namespace plant_nanny::services::ota
