#include "libs/plant_nanny/services/captors/humidity/Humidity.h"
#include <Arduino.h>
#include <cmath>

namespace plant_nanny::services::captors::humidity
{

void Humidity::initialize(uint8_t adcPin)
{
    _adcPin = adcPin;
    _hasPowerControl = false;
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    
    _initialized = true;
}

void Humidity::initialize(uint8_t adcPin, uint8_t powerPin)
{
    _adcPin = adcPin;
    _powerPin = powerPin;
    _hasPowerControl = true;
    
    pinMode(_powerPin, OUTPUT);
    digitalWrite(_powerPin, LOW);
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    
    _initialized = true;
}

void Humidity::powerOn()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, HIGH);
        delay(WARMUP_MS);
    }
}

void Humidity::powerOff()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, LOW);
    }
}

int Humidity::readAdc()
{
    long sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += analogRead(_adcPin);
        delay(2);
    }
    return static_cast<int>(sum / NUM_SAMPLES);
}

void Humidity::calibrate(int dryValue, int wetValue)
{
    _dryValue = dryValue;
    _wetValue = wetValue;
}

float Humidity::adcToHumidity(int adcValue)
{
    if (adcValue < 0)
    {
        return 0.0f;
    }
    
    float humidity;
    
    // Map ADC value to humidity percentage using calibration values
    // Capacitive sensors typically: higher ADC = drier soil
    if (_invertReading)
    {
        // Higher ADC = less humidity (typical capacitive sensor)
        // Map from [wetValue, dryValue] to [100%, 0%]
        humidity = 100.0f * static_cast<float>(_dryValue - adcValue) / static_cast<float>(_dryValue - _wetValue);
    }
    else
    {
        // Higher ADC = more humidity
        // Map from [dryValue, wetValue] to [0%, 100%]
        humidity = 100.0f * static_cast<float>(adcValue - _dryValue) / static_cast<float>(_wetValue - _dryValue);
    }
    
    // Clamp to 0-100 range
    if (humidity < 0.0f) humidity = 0.0f;
    if (humidity > 100.0f) humidity = 100.0f;
    
    return humidity;
}

float Humidity::read()
{
    if (!_initialized)
    {
        return NAN;
    }
    
    powerOn();
    int adcValue = readAdc();
    powerOff();
    
    return adcToHumidity(adcValue);
}

}