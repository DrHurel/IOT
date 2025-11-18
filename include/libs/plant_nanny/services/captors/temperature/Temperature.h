#pragma once

namespace plant_nanny::services::captors::temperature
{
    class Temperature
    {
    private:
        /* data */
    public:
        Temperature() = default;
        ~Temperature() = default;
        Temperature(const Temperature &) = delete;
        Temperature(Temperature &&) = delete;
        Temperature &operator=(const Temperature &) = delete;
        Temperature &operator=(Temperature &&) = delete;
    };

}