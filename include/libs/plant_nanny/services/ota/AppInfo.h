#pragma once

#include "libs/common/patterns/Result.h"
#include "esp_app_format.h"
#include <string>

namespace plant_nanny::services::ota
{
    class AppInfo
    {
    public:
        static common::patterns::Result<std::string> get_version();
        static common::patterns::Result<std::string> get_name();

    private:
        static const esp_app_desc_t* get_description();
    };

} // namespace plant_nanny::services::ota
