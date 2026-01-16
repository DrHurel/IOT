#pragma once

#include "libs/plant_nanny/services/captors/ISensorManager.h"
#include "libs/plant_nanny/services/captors/temperature/Temperature.h"
#include "libs/plant_nanny/services/captors/luminosity/Luminosity.h"
#include "libs/plant_nanny/services/captors/humidity/Humidity.h"

namespace plant_nanny::services::captors
{
    /**
     * @brief Configuration for SensorManager
     */
    struct SensorManagerConfig
    {
        SensorPins pins;
        temperature::ThermistorConfig thermistorConfig;
        
        static SensorManagerConfig defaultConfig()
        {
            SensorManagerConfig config;
            config.pins.thermistorPin = 33;
            config.pins.ldrPin = 37;
            config.pins.powerPin = 26;
            config.thermistorConfig.nominalResistance = 10000.0f;
            config.thermistorConfig.nominalTemperature = 25.0f;
            config.thermistorConfig.betaCoefficient = 3950.0f;
            config.thermistorConfig.seriesResistance = 5600.0f;
            return config;
        }
    };

    class SensorManager : public ISensorManager
    {
    private:
        temperature::Temperature _temperature;
        luminosity::Luminosity _luminosity;
        humidity::Humidity _humidity;
        SensorPins _pins;
        bool _initialized = false;

    public:
        SensorManager();
        explicit SensorManager(const SensorManagerConfig& config);
        ~SensorManager() override = default;
        
        SensorManager(const SensorManager&) = delete;
        SensorManager(SensorManager&&) = delete;
        SensorManager& operator=(const SensorManager&) = delete;
        SensorManager& operator=(SensorManager&&) = delete;

        void initialize() override;
        void initialize(const SensorPins& pins) override;
        void configureThermistor(const temperature::ThermistorConfig& config) override;
        SensorData read() override;
        
        temperature::Temperature& temperature() { return _temperature; }
        luminosity::Luminosity& luminosity() { return _luminosity; }
        humidity::Humidity& humidity() { return _humidity; }
        
        bool isInitialized() const { return _initialized; }
    };

}
