#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include <cstddef>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_app_format.h"

typedef uint32_t esp_ota_handle_t;

typedef enum
{
    ESP_OTA_IMG_NEW = 0,
    ESP_OTA_IMG_PENDING_VERIFY = 1,
    ESP_OTA_IMG_VALID = 2,
    ESP_OTA_IMG_INVALID = 3,
    ESP_OTA_IMG_ABORTED = 4,
} esp_ota_img_states_t;

static constexpr uint32_t OTA_WITH_SEQUENTIAL_WRITES = 0xFFFFFFFF;

// Mock implementations
inline esp_err_t esp_ota_begin(const esp_partition_t *partition, size_t image_size, esp_ota_handle_t *out_handle)
{
    if (partition == nullptr || out_handle == nullptr)
        return ESP_ERR_INVALID_ARG;

    static uint32_t handle_counter = 1;
    *out_handle = handle_counter++;
    return ESP_OK;
}

inline esp_err_t esp_ota_write(esp_ota_handle_t handle, const void *data, size_t size)
{
    if (handle == 0 || data == nullptr || size == 0)
        return ESP_ERR_INVALID_ARG;

    return ESP_OK;
}

inline esp_err_t esp_ota_end(esp_ota_handle_t handle)
{
    if (handle == 0)
        return ESP_ERR_INVALID_ARG;

    return ESP_OK;
}

inline esp_err_t esp_ota_abort(esp_ota_handle_t handle)
{
    if (handle == 0)
        return ESP_ERR_INVALID_ARG;

    return ESP_OK;
}

inline esp_err_t esp_ota_set_boot_partition(const esp_partition_t *partition)
{
    if (partition == nullptr)
        return ESP_ERR_INVALID_ARG;

    return ESP_OK;
}

inline esp_err_t esp_ota_get_state_partition(const esp_partition_t *partition, esp_ota_img_states_t *ota_state)
{
    if (partition == nullptr || ota_state == nullptr)
        return ESP_ERR_INVALID_ARG;

    *ota_state = ESP_OTA_IMG_VALID;
    return ESP_OK;
}

inline esp_err_t esp_ota_mark_app_valid_cancel_rollback(void)
{
    return ESP_OK;
}

inline esp_err_t esp_ota_mark_app_invalid_rollback_and_reboot(void)
{
    // In mock, don't actually reboot
    return ESP_OK;
}

#else
#include_next <esp_ota_ops.h>
#endif
