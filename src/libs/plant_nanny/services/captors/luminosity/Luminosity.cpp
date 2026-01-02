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

namespace plant_nanny::services::captors::luminosity
{

const int analogPin = 36; // ESP32 pin GPIO36 (ADC0)
const float adcMax = 4095.0f; // 12-bit ADC on ESP32

// Convert raw ADC reading to brightness percentage (0..100)
static float sensorValueToPercent(int sensorValue)
{
  if (sensorValue < 0) return NAN;
  if (sensorValue > static_cast<int>(adcMax)) sensorValue = static_cast<int>(adcMax);
  float p = (static_cast<float>(sensorValue) / adcMax) * 100.0f;
  if (p < 0.0f) p = 0.0f;
  if (p > 100.0f) p = 100.0f;
  return p;
}

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

#if defined(ARDUINO_ARCH_ESP32)
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
#endif
}

void loop() {
  // reads the input on analog pin (value between 0 and 4095)
  int analogValue = analogRead(analogPin);
  float brightness = sensorValueToPercent(analogValue);

  Serial.print("Analog Value = ");
  Serial.print(analogValue);   // the raw analog reading
  Serial.print(" | ");

  if (!std::isfinite(brightness)) {
    Serial.println("Brightness: N/A");
  } else {
    Serial.print("Brightness: ");
    Serial.print(brightness, 1);
    Serial.print(" %");

    // Qualitative thresholds (tweakable per sensor / environment)
    if (brightness < 1.0f) {
      Serial.println(" => Dark");
    } else if (brightness < 20.0f) {
      Serial.println(" => Dim");
    } else if (brightness < 50.0f) {
      Serial.println(" => Light");
    } else if (brightness < 80.0f) {
      Serial.println(" => Bright");
    } else {
      Serial.println(" => Very bright");
    }
  }

  delay(500);
}

}