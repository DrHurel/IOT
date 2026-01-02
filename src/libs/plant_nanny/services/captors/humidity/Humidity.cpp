#if defined(ARDUINO) || defined(ARDUINO_ARCH_ESP32) 
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

namespace plant_nanny::services::captors::humidity
{

const int analogPin = 33;   // ESP32 ADC pin (AO output of soil sensor)
const float adcMax = 4095.0f; // 12-bit ADC on ESP32

// Convert raw ADC value to moisture percentage (0..100).
// Assumes lower raw -> wetter (adjust formula if your sensor is inverted)
static float sensorValueToPercent(int sensorValue)
{
  if (sensorValue < 0) {
    return NAN;
  }
  if (sensorValue > static_cast<int>(adcMax)) {
    sensorValue = static_cast<int>(adcMax);
  }

  float raw = static_cast<float>(sensorValue);
  float percent = (1.0f - (raw / adcMax)) * 100.0f;

  if (percent < 0.0f) percent = 0.0f;
  if (percent > 100.0f) percent = 100.0f;
  return percent;
}

void setup() {
  Serial.begin(115200);
  #if defined(ARDUINO_ARCH_ESP32)
  analogSetAttenuation(ADC_11db); // allows reading up to ~3.3V
  analogReadResolution(12);
  #endif
}

void loop() {
  int soilValue = analogRead(analogPin);
  float moisture = sensorValueToPercent(soilValue);

  Serial.print("Soil moisture raw value: ");
  Serial.println(soilValue);

  if (!std::isfinite(moisture)) {
    Serial.println("Status: N/A");
  } else {
    Serial.print("Moisture: ");
    Serial.print(moisture, 1);
    Serial.print(" % - Status: ");

    if (moisture > 80.0f) {
      Serial.println("Very Wet");
    } else if (moisture > 60.0f) {
      Serial.println("Wet");
    } else if (moisture > 30.0f) {
      Serial.println("Dry");
    } else {
      Serial.println("Very Dry");
    }
  }

  Serial.println("------------------------");
  delay(1000);
}

}