#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include "esp_err.h"

typedef enum
{
    ESP_PARTITION_TYPE_APP = 0x00,
    ESP_PARTITION_TYPE_DATA = 0x01,
} esp_partition_type_t;

typedef enum
{
    ESP_PARTITION_SUBTYPE_APP_FACTORY = 0x00,
    ESP_PARTITION_SUBTYPE_APP_OTA_0 = 0x10,
    ESP_PARTITION_SUBTYPE_APP_OTA_1 = 0x11,
    ESP_PARTITION_SUBTYPE_APP_OTA_2 = 0x12,
    ESP_PARTITION_SUBTYPE_APP_OTA_3 = 0x13,
    ESP_PARTITION_SUBTYPE_APP_OTA_4 = 0x14,
    ESP_PARTITION_SUBTYPE_APP_OTA_5 = 0x15,
} esp_partition_subtype_t;

typedef struct esp_partition
{
    esp_partition_type_t type;
    esp_partition_subtype_t subtype;
    uint32_t address;
    uint32_t size;
    char label[17];
    bool encrypted;
} esp_partition_t;

// Mock implementations
inline const esp_partition_t *esp_ota_get_boot_partition(void)
{
    static esp_partition_t mock_partition = {
        ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_APP_OTA_0,
        0x10000,
        0x100000,
        "ota_0",
        false};
    return &mock_partition;
}

inline const esp_partition_t *esp_ota_get_running_partition(void)
{
    return esp_ota_get_boot_partition();
}

inline const esp_partition_t *esp_ota_get_next_update_partition(const esp_partition_t *start)
{
    static esp_partition_t mock_partition = {
        ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_APP_OTA_1,
        0x110000,
        0x100000,
        "ota_1",
        false};
    return &mock_partition;
}

#else
#include_next <esp_partition.h>
#endif
