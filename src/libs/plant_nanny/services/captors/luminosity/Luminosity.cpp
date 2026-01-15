#include "libs/plant_nanny/services/captors/luminosity/Luminosity.h"
#include <Arduino.h>
#include <cmath>

namespace plant_nanny::services::captors::luminosity
{

void Luminosity::initialize(uint8_t adcPin)
{
    _adcPin = adcPin;
    _hasPowerControl = false;
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    
    _initialized = true;
}

void Luminosity::initialize(uint8_t adcPin, uint8_t powerPin)
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

void Luminosity::powerOn()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, HIGH);
        delay(WARMUP_MS);
    }
}

void Luminosity::powerOff()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, LOW);
    }
}

int Luminosity::readAdc()
{
    long sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += analogRead(_adcPin);
        delay(2);
    }
    return static_cast<int>(sum / NUM_SAMPLES);
}

float Luminosity::adcToLuminosity(int adcValue)
{
    if (adcValue < 0)
    {
        return 0.0f;
    }
    
    float luminosity;
    if (_invertReading)
    {
        // Higher ADC = less light (LDR between ADC and GND with pull-up)
        luminosity = 100.0f * (1.0f - static_cast<float>(adcValue) / ADC_MAX);
    }
    else
    {
        // Higher ADC = more light (LDR between VCC and ADC with pull-down)
        luminosity = 100.0f * static_cast<float>(adcValue) / ADC_MAX;
    }
    
    // Clamp to 0-100 range
    if (luminosity < 0.0f) luminosity = 0.0f;
    if (luminosity > 100.0f) luminosity = 100.0f;
    
    return luminosity;
}

float Luminosity::read()
{
    if (!_initialized)
    {
        return NAN;
    }
    
    powerOn();
    int adcValue = readAdc();
    powerOff();
    
    return adcToLuminosity(adcValue);
}

}
