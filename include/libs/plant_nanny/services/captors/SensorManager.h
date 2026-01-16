#pragma once

#include "libs/plant_nanny/services/captors/ISensorManager.h"
#include "libs/plant_nanny/services/captors/temperature/Temperature.h"
#include "libs/plant_nanny/services/captors/luminosity/Luminosity.h"
#include "libs/plant_nanny/services/captors/humidity/Humidity.h"

namespace plant_nanny::services::captors
{
    class SensorManager : public ISensorManager
    {
    private:
        temperature::Temperature _temperature;
        luminosity::Luminosity _luminosity;
        humidity::Humidity _humidity;
        SensorPins _pins;
        bool _initialized = false;

    public:
        SensorManager() = default;
        ~SensorManager() override = default;
        
        SensorManager(const SensorManager&) = delete;
        SensorManager(SensorManager&&) = delete;
        SensorManager& operator=(const SensorManager&) = delete;
        SensorManager& operator=(SensorManager&&) = delete;

        void initialize() override;
        void initialize(const SensorPins& pins) override;
        void configureThermistor(const temperature::ThermistorConfig& config) override;
        SensorData read() override;
        
        /**
         * @brief Access temperature sensor directly
         */
        temperature::Temperature& temperature() { return _temperature; }
        
        /**
         * @brief Access luminosity sensor directly
         */
        luminosity::Luminosity& luminosity() { return _luminosity; }
        
        /**
         * @brief Access humidity sensor directly
         */
        humidity::Humidity& humidity() { return _humidity; }
        
        bool isInitialized() const { return _initialized; }
    };

}
