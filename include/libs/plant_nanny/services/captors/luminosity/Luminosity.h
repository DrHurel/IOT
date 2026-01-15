#pragma once

#include <cstdint>

namespace plant_nanny::services::captors::luminosity
{
    /**
     * @brief Luminosity sensor using LDR (Light Dependent Resistor)
     * 
     * Reads light level from an LDR connected to an ADC pin.
     * Returns luminosity as percentage (0-100%)
     */
    class Luminosity
    {
    private:
        uint8_t _adcPin = 36;
        uint8_t _powerPin = 0;
        bool _hasPowerControl = false;
        bool _initialized = false;
        bool _invertReading = false;  // If true, higher ADC = less light
        
        static constexpr int ADC_MAX = 4095;
        static constexpr uint32_t WARMUP_MS = 50;
        static constexpr int NUM_SAMPLES = 10;
        
        int readAdc();
        float adcToLuminosity(int adcValue);
        void powerOn();
        void powerOff();

    public:
        Luminosity() = default;
        ~Luminosity() = default;
        Luminosity(const Luminosity &) = delete;
        Luminosity(Luminosity &&) = delete;
        Luminosity &operator=(const Luminosity &) = delete;
        Luminosity &operator=(Luminosity &&) = delete;
        
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
         * @param invert If true, higher ADC = less light (LDR to GND config)
         */
        void setInverted(bool invert) { _invertReading = invert; }
        
        /**
         * @brief Read luminosity
         * @return Luminosity percentage (0-100), or NaN if error
         */
        float read();
        
        /**
         * @brief Check if sensor is initialized
         */
        bool isInitialized() const { return _initialized; }
    };
}
