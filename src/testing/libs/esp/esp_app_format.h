#pragma once

#ifdef NATIVE_TEST

#include <cstdint>
#include "esp_err.h"

static constexpr uint32_t ESP_APP_DESC_MAGIC_WORD = 0xABCD5432;

typedef struct
{
    uint32_t magic_word;
    uint32_t secure_version;
    uint32_t reserv1[2];
    char version[32];
    char project_name[32];
    char time[16];
    char date[16];
    char idf_ver[32];
    uint8_t app_elf_sha256[32];
    uint32_t reserv2[20];
} esp_app_desc_t;

inline const esp_app_desc_t *esp_app_get_description(void)
{
    static esp_app_desc_t mock_desc = {
        ESP_APP_DESC_MAGIC_WORD,
        0,
        {0, 0},
        "1.0.0",
        "PlantNanny",
        "00:00:00",
        "Nov 23 2025",
        "v5.0",
        {0},
        {0}};
    return &mock_desc;
}

#else
#include_next <esp_app_format.h>
#endif
