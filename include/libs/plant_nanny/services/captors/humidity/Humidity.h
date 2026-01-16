#pragma once

#include <cstdint>

namespace plant_nanny::services::captors::humidity
{
    /**
     * @brief Soil humidity sensor using capacitive or resistive probe
     * 
     * Reads soil moisture level from a sensor connected to an ADC pin.
     * Returns humidity as percentage (0-100%)
     * 0% = dry soil, 100% = wet soil
     */
    class Humidity
    {
    private:
        uint8_t _adcPin = 32;
        uint8_t _powerPin = 0;
        bool _hasPowerControl = false;
        bool _initialized = false;
        bool _invertReading = true;  // If true, higher ADC = less humidity (typical for capacitive sensors)
        
        static constexpr int ADC_MAX = 4095;
        static constexpr uint32_t WARMUP_MS = 50;
        static constexpr int NUM_SAMPLES = 10;
        
        // Calibration values (can be adjusted per sensor)
        int _dryValue = 4095;   // ADC value when sensor is in dry air
        int _wetValue = 1500;   // ADC value when sensor is in water
        
        int readAdc();
        float adcToHumidity(int adcValue);
        void powerOn();
        void powerOff();

    public:
        Humidity() = default;
        ~Humidity() = default;
        Humidity(const Humidity &) = delete;
        Humidity(Humidity &&) = delete;
        Humidity &operator=(const Humidity &) = delete;
        Humidity &operator=(Humidity &&) = delete;
        
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
         * @brief Set whether to invert the reading
         * @param invert If true, higher ADC = less humidity
         */
        void setInverted(bool invert) { _invertReading = invert; }
        
        /**
         * @brief Calibrate the sensor with known dry and wet values
         * @param dryValue ADC value when sensor is dry
         * @param wetValue ADC value when sensor is wet
         */
        void calibrate(int dryValue, int wetValue);
        
        /**
         * @brief Read humidity
         * @return Humidity percentage (0-100), or NaN if error
         */
        float read();
        
        /**
         * @brief Check if sensor is initialized
         */
        bool isInitialized() const { return _initialized; }
    };
}