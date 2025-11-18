#pragma once

namespace plant_nanny::services::captors::humidity
{
    class Humidity
    {
    private:
        /* data */
    public:
        Humidity() = default;
        ~Humidity() = default;
        Humidity(const Humidity &) = delete;
        Humidity(Humidity &&) = delete;
        Humidity &operator=(const Humidity &) = delete;
        Humidity &operator=(Humidity &&) = delete;
    };
}