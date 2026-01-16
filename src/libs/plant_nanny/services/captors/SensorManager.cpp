#include "libs/plant_nanny/services/captors/SensorManager.h"
#include <Arduino.h>
#include <cmath>

namespace plant_nanny::services::captors
{

void SensorManager::initialize()
{
    initialize(SensorPins{});
}

void SensorManager::initialize(const SensorPins& pins)
{
    _pins = pins;
    
    // Configure power control pin (SensorManager owns this)
    pinMode(_pins.powerPin, OUTPUT);
    digitalWrite(_pins.powerPin, LOW);
    
    // Initialize sensors WITHOUT power control (we manage it here)
    _temperature.initialize(_pins.thermistorPin);
    _luminosity.initialize(_pins.ldrPin);
    _humidity.initialize(_pins.humidityPin);
    
    _initialized = true;
}

void SensorManager::configureThermistor(const temperature::ThermistorConfig& config)
{
    _temperature.configure(config);
}

SensorData SensorManager::read()
{
    SensorData data;
    
    if (!_initialized)
    {
        data.valid = false;
        return data;
    }
    
    // Power on sensors
    digitalWrite(_pins.powerPin, HIGH);
    delay(50);  // Warm-up time
    
    // Read all sensors
    data.temperatureC = _temperature.read();
    data.luminosityPct = _luminosity.read();
    data.humidityPct = _humidity.read();
    
    // Power off sensors
    digitalWrite(_pins.powerPin, LOW);
    
    data.valid = !std::isnan(data.temperatureC) || !std::isnan(data.luminosityPct) || !std::isnan(data.humidityPct);
    
    return data;
}

}
