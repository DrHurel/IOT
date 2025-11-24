#include "libs/common/utils/EspError.h"

namespace common::utils
{
    patterns::Result<void> EspError::to_result(esp_err_t err, const std::string& context)
    {
        if (err == ESP_OK)
        {
            return patterns::Result<void>::success();
        }
        
        std::string message = context + ": " + std::string(esp_err_to_name(err));
        return patterns::Result<void>::failure(patterns::Error(message));
    }

    bool EspError::is_ok(esp_err_t err)
    {
        return err == ESP_OK;
    }

    std::string EspError::error_name(esp_err_t err)
    {
        return std::string(esp_err_to_name(err));
    }

} // namespace common::utils
