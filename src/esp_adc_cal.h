#pragma once

#include <cstdint>

// Simulator stub for ESP-IDF's esp_adc_cal.h, mirrored onto the Arduino-ESP32
// analogRead surface the firmware's ADC consumers use (InputManager's button
// ladder). On the host there is no ADC: reads return 0 (ladder fully pressed
// side reads as an unpressed high rail) and calibration is a no-op.
enum adc_atten_t {
  ADC_ATTEN_DB_0 = 0,
  ADC_ATTEN_DB_2_5 = 1,
  ADC_ATTEN_DB_6 = 2,
  ADC_ATTEN_DB_11 = 3,
  ADC_11db = ADC_ATTEN_DB_11,  // Arduino-ESP32 legacy spelling
};

struct esp_adc_cal_characteristics_t {
  uint32_t vref = 1100;
  uint32_t atten = ADC_ATTEN_DB_11;
};

inline void analogSetAttenuation(adc_atten_t) {}

inline uint32_t analogReadMilliVolts(uint8_t) { return 0; }
