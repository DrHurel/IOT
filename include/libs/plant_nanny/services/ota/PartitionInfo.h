#pragma once

#include "libs/common/patterns/Result.h"
#include "esp_partition.h"
#include <string>

namespace plant_nanny::services::ota
{
    class PartitionInfo
    {
    public:
        static const esp_partition_t *get_boot_partition();
        static const esp_partition_t *get_running_partition();
        static const esp_partition_t *get_next_update_partition();
        static bool partitions_match(const esp_partition_t *p1, const esp_partition_t *p2);
        static common::patterns::Result<const esp_partition_t *> get_update_partition();
    };

} // namespace plant_nanny::services::ota
