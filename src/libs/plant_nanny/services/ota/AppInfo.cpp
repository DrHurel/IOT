#include "libs/plant_nanny/services/ota/AppInfo.h"

namespace plant_nanny::services::ota
{
    const common::patterns::Result<std::string> AppInfo::get_version()
    {
        const esp_app_desc_t *app_desc = get_description();
        if (app_desc == nullptr)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("Failed to get app description"));
        }
        return common::patterns::Result<std::string>::success(std::string(app_desc->version));
    }

    const common::patterns::Result<std::string> AppInfo::get_name()
    {
        const esp_app_desc_t *app_desc = get_description();
        if (app_desc == nullptr)
        {
            return common::patterns::Result<std::string>::failure(
                common::patterns::Error("Failed to get app description"));
        }
        return common::patterns::Result<std::string>::success(std::string(app_desc->project_name));
    }

    const esp_app_desc_t *AppInfo::get_description()
    {
        const esp_partition_t *running = esp_ota_get_running_partition();
        if (running == nullptr)
        {
            return nullptr;
        }

        static esp_app_desc_t app_desc;
        if (esp_ota_get_partition_description(running, &app_desc) != ESP_OK)
        {
            return nullptr;
        }

        return &app_desc;
    }

} // namespace plant_nanny::services::ota
