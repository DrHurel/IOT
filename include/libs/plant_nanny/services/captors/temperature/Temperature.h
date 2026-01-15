#pragma once

#include <cstdint>

namespace plant_nanny::services::captors::temperature
{
    /**
     * @brief Thermistor configuration for temperature calculation
     * Using Steinhart-Hart Beta model
     */
    struct ThermistorConfig
    {
        float nominalResistance = 10000.0f;  // Resistance at nominal temperature (Ohms)
        float nominalTemperature = 25.0f;    // Nominal temperature (Celsius)
        float betaCoefficient = 3950.0f;     // Beta coefficient of thermistor
        float seriesResistance = 10000.0f;   // Series resistor value (Ohms)
    };

    /**
     * @brief Temperature sensor using NTC thermistor
     * 
     * Reads temperature from a thermistor connected to an ADC pin.
     * Wiring: VCC -- Series Resistor -- ADC Pin -- Thermistor -- GND
     */
    class Temperature
    {
    private:
        uint8_t _adcPin = 35;
        uint8_t _powerPin = 0;
        bool _hasPowerControl = false;
        bool _initialized = false;
        ThermistorConfig _config;
        
        static constexpr int ADC_MAX = 4095;
        static constexpr uint32_t WARMUP_MS = 50;
        static constexpr int NUM_SAMPLES = 10;
        
        int readAdc();
        float adcToTemperature(int adcValue);
        void powerOn();
        void powerOff();

    public:
        Temperature() = default;
        ~Temperature() = default;
        Temperature(const Temperature &) = delete;
        Temperature(Temperature &&) = delete;
        Temperature &operator=(const Temperature &) = delete;
        Temperature &operator=(Temperature &&) = delete;
        
        /**
         * @brief Initialize with ADC pin only (no power control)
         * @param adcPin GPIO pin for ADC reading
         */
        void initialize(uint8_t adcPin);
        
        /**
         * @brief Initialize with ADC pin and power control
         * @param adcPin GPIO pin for ADC reading
         * @param powerPin GPIO pin for transistor power control
         */
        void initialize(uint8_t adcPin, uint8_t powerPin);
        
        /**
         * @brief Configure thermistor parameters
         * @param config Thermistor configuration
         */
        void configure(const ThermistorConfig& config);
        
        /**
         * @brief Read temperature
         * @return Temperature in Celsius, or NaN if error
         */
        float read();
        
        /**
         * @brief Check if sensor is initialized
         */
        bool isInitialized() const { return _initialized; }
    };

}
