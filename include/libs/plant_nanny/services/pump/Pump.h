#pragma once

#include "libs/plant_nanny/services/pump/IPump.h"
#include <cstdint>

namespace plant_nanny::services::pump
{
    /**
     * @brief Controls a pump (or LED indicator) via transistor
     * 
     * The pump is controlled via a transistor connected to a GPIO pin.
     * HIGH activates the pump, LOW deactivates it.
     */
    class Pump : public IPump
    {
    private:
        uint8_t _controlPin;
        bool _active = false;

    public:
        explicit Pump(uint8_t controlPin);
        ~Pump() override = default;

        void activate() override;
        void deactivate() override;
        void toggle() override;
        bool isActive() const override { return _active; }
        void setActive(bool active) override;
    };

} // namespace plant_nanny::services::pump
