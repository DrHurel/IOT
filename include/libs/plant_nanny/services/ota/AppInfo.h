#pragma once

#include "libs/common/patterns/Result.h"
#include <esp_ota_ops.h>
#include <string>

namespace plant_nanny::services::ota
{
    class AppInfo
    {
    public:
        static const common::patterns::Result<std::string> get_version();
        static const common::patterns::Result<std::string> get_name();

    private:
        static const esp_app_desc_t* get_description();
    };

} // namespace plant_nanny::services::ota
