#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32) || __has_include(<Arduino.h>)
#include <Arduino.h>
#else
#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>

// Minimal Arduino-like stubs for host build / unit tests
namespace {
struct SerialStub {
  void begin(int) {}
  void print(const char* s) { std::cout << s; }
  void print(const std::string& s) { std::cout << s; }
  void print(int v) { std::cout << v; }
  void print(float v) { std::cout << v; }
  void print(float v, int prec) { if (prec >= 0) { std::cout << std::fixed << std::setprecision(prec) << v << std::defaultfloat; } else { std::cout << v; } }
  void println(const char* s = "") { std::cout << s << std::endl; }
  void println(const std::string& s) { std::cout << s << std::endl; }
  void println(int v) { std::cout << v << std::endl; }
  void println(float v) { std::cout << v << std::endl; }
} Serial;

inline int analogRead(int /*pin*/) { return 2000; } // stub: mid-range value
inline void analogSetAttenuation(int) {}
inline void analogReadResolution(int) {}
inline void delay(unsigned long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
}
#endif

#include <cmath>

namespace plant_nanny::services::captors::temperature
{

const int analogPin = 34; // Analog pin connected to the NTC thermistor
const float resistorValue = 10000.0f; // Resistor value in ohms (R0)
// ADC max value: change to 1023 if using 10-bit ADC platforms
const float adcMax = 4095.0f;

// Read thermistor resistance from ADC value. Returns INFINITY on invalid reading.
static float sensorValueToResistance(int sensorValue)
{
  if (sensorValue <= 0) {
    return INFINITY; // avoid division by zero
  }
  return resistorValue * ( (adcMax / static_cast<float>(sensorValue)) - 1.0f );
}

// Convert resistance to temperature (Celsius) using B-parameter (Steinhart-Hart simplified)
static float resistanceToCelsius(float resistance)
{
  if (!std::isfinite(resistance) || resistance <= 0.0f) {
    return NAN;
  }

  const float B = 3950.0f; // Beta parameter
  const float T0 = 25.0f;  // Reference temperature in °C
  const float R0 = 10000.0f; // Reference resistance at T0

  // 1/T = 1/T0 + (1/B)*ln(R/R0)
  float invT = 1.0f / (T0 + 273.15f) + (1.0f / B) * std::log(resistance / R0);
  float temperatureK = 1.0f / invT;
  return temperatureK - 273.15f; // convert Kelvin to Celsius
}

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  analogSetAttenuation(ADC_11db); 
  analogReadResolution(12);
#endif
}

void loop() {
  int sensorValue = analogRead(analogPin);
  float resistance = sensorValueToResistance(sensorValue);
  float temperatureC = resistanceToCelsius(resistance);

  if (std::isnan(temperatureC)) {
    Serial.println("Temperature: N/A");
  } else {
    float temperatureF = temperatureC * 9.0f / 5.0f + 32.0f;
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" °C | ");
    Serial.print(temperatureF);
    Serial.println(" °F");
  }

  delay(1000); 
}

}