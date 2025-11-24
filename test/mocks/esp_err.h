#pragma once

#ifdef NATIVE_TEST

#include <cstdint>

typedef int esp_err_t;

enum class EspError : esp_err_t {
    OK = 0,
    FAIL = -1,
    NO_MEM = 0x101,
    INVALID_ARG = 0x102,
    INVALID_STATE = 0x103,
    INVALID_SIZE = 0x104,
    NOT_FOUND = 0x105,
    NOT_SUPPORTED = 0x106,
    TIMEOUT = 0x107,
    OTA_VALIDATE_FAILED = 0x1501
};

// Legacy defines for compatibility with ESP-IDF code
static constexpr esp_err_t ESP_OK = static_cast<esp_err_t>(EspError::OK);
static constexpr esp_err_t ESP_FAIL = static_cast<esp_err_t>(EspError::FAIL);
static constexpr esp_err_t ESP_ERR_NO_MEM = static_cast<esp_err_t>(EspError::NO_MEM);
static constexpr esp_err_t ESP_ERR_INVALID_ARG = static_cast<esp_err_t>(EspError::INVALID_ARG);
static constexpr esp_err_t ESP_ERR_INVALID_STATE = static_cast<esp_err_t>(EspError::INVALID_STATE);
static constexpr esp_err_t ESP_ERR_INVALID_SIZE = static_cast<esp_err_t>(EspError::INVALID_SIZE);
static constexpr esp_err_t ESP_ERR_NOT_FOUND = static_cast<esp_err_t>(EspError::NOT_FOUND);
static constexpr esp_err_t ESP_ERR_NOT_SUPPORTED = static_cast<esp_err_t>(EspError::NOT_SUPPORTED);
static constexpr esp_err_t ESP_ERR_TIMEOUT = static_cast<esp_err_t>(EspError::TIMEOUT);
static constexpr esp_err_t ESP_ERR_OTA_VALIDATE_FAILED = static_cast<esp_err_t>(EspError::OTA_VALIDATE_FAILED);

inline const char* esp_err_to_name(esp_err_t err)
{
    switch (err)
    {
        case ESP_OK: return "ESP_OK";
        case ESP_FAIL: return "ESP_FAIL";
        case ESP_ERR_NO_MEM: return "ESP_ERR_NO_MEM";
        case ESP_ERR_INVALID_ARG: return "ESP_ERR_INVALID_ARG";
        case ESP_ERR_INVALID_STATE: return "ESP_ERR_INVALID_STATE";
        case ESP_ERR_INVALID_SIZE: return "ESP_ERR_INVALID_SIZE";
        case ESP_ERR_NOT_FOUND: return "ESP_ERR_NOT_FOUND";
        case ESP_ERR_NOT_SUPPORTED: return "ESP_ERR_NOT_SUPPORTED";
        case ESP_ERR_TIMEOUT: return "ESP_ERR_TIMEOUT";
        case ESP_ERR_OTA_VALIDATE_FAILED: return "ESP_ERR_OTA_VALIDATE_FAILED";
        default: return "UNKNOWN_ERROR";
    }
}

#else
#include_next <esp_err.h>
#endif
