#include "libs/plant_nanny/services/captors/temperature/Temperature.h"
#include <Arduino.h>
#include <cmath>

namespace plant_nanny::services::captors::temperature
{

void Temperature::initialize(uint8_t adcPin)
{
    _adcPin = adcPin;
    _hasPowerControl = false;
    
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    
    _initialized = true;
}

void Temperature::initialize(uint8_t adcPin, uint8_t powerPin)
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

void Temperature::configure(const ThermistorConfig& config)
{
    _config = config;
}

void Temperature::powerOn()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, HIGH);
        delay(WARMUP_MS);
    }
}

void Temperature::powerOff()
{
    if (_hasPowerControl)
    {
        digitalWrite(_powerPin, LOW);
    }
}

int Temperature::readAdc()
{
    long sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += analogRead(_adcPin);
        delay(2);
    }
    return static_cast<int>(sum / NUM_SAMPLES);
}

float Temperature::adcToTemperature(int adcValue)
{
    if (adcValue <= 10 || adcValue >= ADC_MAX - 10)
    {
        return NAN;
    }
    
    // Voltage divider: VCC -- Thermistor -- ADC -- Series Resistor -- GND
    // Rtherm = Rseries * (ADC_MAX - adcValue) / adcValue
    float resistance = _config.seriesResistance * 
                       (static_cast<float>(ADC_MAX - adcValue) / adcValue);
    
    // Steinhart-Hart Beta equation: 1/T = 1/T0 + (1/B) * ln(R/R0)
    float steinhart;
    steinhart = log(resistance / _config.nominalResistance);
    steinhart /= _config.betaCoefficient;
    steinhart += 1.0f / (_config.nominalTemperature + 273.15f);
    steinhart = 1.0f / steinhart;
    steinhart -= 273.15f;
    
    return steinhart;
}

float Temperature::read()
{
    if (!_initialized)
    {
        return NAN;
    }
    
    powerOn();
    int adcValue = readAdc();
    powerOff();
    
    return adcToTemperature(adcValue);
}

}
