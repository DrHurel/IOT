#include "libs/plant_nanny/services/ota/AppInfo.h"

namespace plant_nanny::services::ota
{
    common::patterns::Result<std::string> AppInfo::get_version()
    {
        const esp_app_desc_t* app_desc = get_description();
        if (app_desc == nullptr)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("Failed to get app description")
            );
        }
        return common::patterns::Result<std::string>::success(std::string(app_desc->version));
    }

    common::patterns::Result<std::string> AppInfo::get_name()
    {
        const esp_app_desc_t* app_desc = get_description();
        if (app_desc == nullptr)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("Failed to get app description")
            );
        }
        return common::patterns::Result<std::string>::success(std::string(app_desc->project_name));
    }

    const esp_app_desc_t* AppInfo::get_description()
    {
        return esp_app_get_description();
    }

} // namespace plant_nanny::services::ota
