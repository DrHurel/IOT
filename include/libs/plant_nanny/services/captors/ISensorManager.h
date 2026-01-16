#pragma once

#include "libs/plant_nanny/services/captors/temperature/Temperature.h"
#include <cstdint>

namespace plant_nanny::services::captors
{
    struct SensorData
    {
        float temperatureC = 0.0f;
        float luminosityPct = 0.0f;
        float humidityPct = 0.0f;
        bool valid = false;
    };

    struct SensorPins
    {
        uint8_t thermistorPin = 33;
        uint8_t ldrPin = 36;
        uint8_t humidityPin = 32;
        uint8_t powerPin = 26;
    };

    /**
     * @brief Interface for sensor management (DIP - Dependency Inversion Principle)
     */
    class ISensorManager
    {
    public:
        virtual ~ISensorManager() = default;

        virtual void initialize() = 0;
        virtual void initialize(const SensorPins& pins) = 0;
        virtual void configureThermistor(const temperature::ThermistorConfig& config) = 0;
        virtual SensorData read() = 0;
    };

} // namespace plant_nanny::services::captors
