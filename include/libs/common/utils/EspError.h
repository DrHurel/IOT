#pragma once

#include "libs/common/patterns/Result.h"
#include "esp_err.h"
#include <string>

namespace common::utils
{
    class EspError
    {
    public:
        static patterns::Result<void> to_result(esp_err_t err, const std::string& context);
        static bool is_ok(esp_err_t err);
        static std::string error_name(esp_err_t err);
    };

} // namespace common::utils
