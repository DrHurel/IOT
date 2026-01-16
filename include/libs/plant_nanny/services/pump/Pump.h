#pragma once

#include <cstdint>

namespace plant_nanny::services::pump
{
    /**
     * @brief Controls a pump (or LED indicator) via transistor
     * 
     * The pump is controlled via a transistor connected to a GPIO pin.
     * HIGH activates the pump, LOW deactivates it.
     */
    class Pump
    {
    private:
        uint8_t _controlPin = 0;
        bool _initialized = false;
        bool _active = false;

    public:
        Pump() = default;
        ~Pump() = default;

        /**
         * @brief Initialize the pump controller
         * @param controlPin GPIO pin connected to transistor base
         */
        void initialize(uint8_t controlPin);

        /**
         * @brief Turn the pump ON
         */
        void activate();

        /**
         * @brief Turn the pump OFF
         */
        void deactivate();

        /**
         * @brief Toggle pump state
         */
        void toggle();

        /**
         * @brief Check if pump is currently active
         */
        bool isActive() const { return _active; }

        /**
         * @brief Set pump state
         * @param active true to activate, false to deactivate
         */
        void setActive(bool active);
    };

} // namespace plant_nanny::services::pump
