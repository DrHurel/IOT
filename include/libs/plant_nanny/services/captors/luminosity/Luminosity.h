#pragma once
namespace plant_nanny::services::captors::luminosity
{
    class Luminosity
    {
    private:
        /* data */
    public:
        Luminosity() = default;
        ~Luminosity() = default;
        Luminosity(const Luminosity &) = delete;
        Luminosity(Luminosity &&) = delete;
        Luminosity &operator=(const Luminosity &) = delete;
        Luminosity &operator=(Luminosity &&) = delete;
    };
}