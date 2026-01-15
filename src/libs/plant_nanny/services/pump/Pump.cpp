#include "libs/plant_nanny/services/pump/Pump.h"
#include <Arduino.h>

namespace plant_nanny::services::pump
{

void Pump::initialize(uint8_t controlPin)
{
    _controlPin = controlPin;
    pinMode(_controlPin, OUTPUT);
    digitalWrite(_controlPin, LOW);
    _active = false;
    _initialized = true;
}

void Pump::activate()
{
    if (!_initialized) return;
    
    digitalWrite(_controlPin, HIGH);
    _active = true;
}

void Pump::deactivate()
{
    if (!_initialized) return;
    
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
