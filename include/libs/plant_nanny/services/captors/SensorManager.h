#pragma once

#include "libs/plant_nanny/services/captors/temperature/Temperature.h"
#include "libs/plant_nanny/services/captors/luminosity/Luminosity.h"

namespace plant_nanny::services::captors
{
    /**
     * @brief Sensor reading data structure
     */
    struct SensorData
    {
        float temperatureC = 0.0f;
        float luminosityPct = 0.0f;
        bool valid = false;
    };

    /**
     * @brief Pin configuration for sensors
     */
    struct SensorPins
    {
        uint8_t thermistorPin = 33;     // ADC pin for thermal resistance
        uint8_t ldrPin = 36;            // ADC pin for light-dependent resistance
        uint8_t powerPin = 26;          // Transistor control pin to power sensors
    };

    /**
     * @brief Manages temperature and luminosity sensors
     * 
     * Wrapper around Temperature and Luminosity classes for convenience.
     * Both sensors share the same power control pin.
     */
    class SensorManager
    {
    private:
        temperature::Temperature _temperature;
        luminosity::Luminosity _luminosity;
        SensorPins _pins;
        bool _initialized = false;

    public:
        SensorManager() = default;
        ~SensorManager() = default;
        
        SensorManager(const SensorManager&) = delete;
        SensorManager(SensorManager&&) = delete;
        SensorManager& operator=(const SensorManager&) = delete;
        SensorManager& operator=(SensorManager&&) = delete;
        
        /**
         * @brief Initialize with default pins
         */
        void initialize();
        
        /**
         * @brief Initialize with custom pins
         */
        void initialize(const SensorPins& pins);
        
        /**
         * @brief Configure thermistor parameters
         */
        void configureThermistor(const temperature::ThermistorConfig& config);
        
        /**
         * @brief Read all sensors
         */
        SensorData read();
        
        /**
         * @brief Access temperature sensor directly
         */
        temperature::Temperature& temperature() { return _temperature; }
        
        /**
         * @brief Access luminosity sensor directly
         */
        luminosity::Luminosity& luminosity() { return _luminosity; }
        
        bool isInitialized() const { return _initialized; }
    };

}
