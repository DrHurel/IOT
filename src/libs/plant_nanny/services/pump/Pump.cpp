#include "libs/plant_nanny/services/pump/Pump.h"
#include <Arduino.h>

namespace plant_nanny::services::pump
{

Pump::Pump(uint8_t controlPin) : _controlPin(controlPin), _active(false)
{
    pinMode(_controlPin, OUTPUT);
    digitalWrite(_controlPin, LOW);
}

void Pump::activate()
{
    digitalWrite(_controlPin, HIGH);
    _active = true;
}

void Pump::deactivate()
{
    digitalWrite(_controlPin, LOW);
    _active = false;
}

void Pump::toggle()
{
    if (_active)
    {
        deactivate();
    }
    else
    {
        activate();
    }
}

void Pump::setActive(bool active)
{
    if (active)
    {
        activate();
    }
    else
    {
        deactivate();
    }
}

} // namespace plant_nanny::services::pump
