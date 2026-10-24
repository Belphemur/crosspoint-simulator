#pragma once

// Stub for ESP-IDF's esp_sleep.h. The simulator doesn't model the ESP32's
// deep-sleep hardware, but firmware main.cpp includes this header.
// The actual sleep entry/exit path goes through HalPowerManager / HalGPIO
// which the simulator implements separately.

#include <cstdint>

typedef enum {
  ESP_SLEEP_WAKEUP_EXT0 = 0,
  ESP_SLEEP_WAKEUP_EXT1 = 1,
  ESP_SLEEP_WAKEUP_TIMER = 2,
  ESP_SLEEP_WAKEUP_TOUCH = 3,
  ESP_SLEEP_WAKEUP_ULP = 4,
  ESP_SLEEP_WAKEUP_GPIO = 5,
  ESP_SLEEP_WAKEUP_UART = 6,
  ESP_SLEEP_WAKEUP_USB = 7,
} esp_sleep_wakeup_cause_t;

static inline void esp_sleep_enable_ext0_wakeup(int, int) {}
static inline void esp_sleep_enable_ext1_wakeup(uint64_t, int) {}
static inline void esp_sleep_enable_timer_wakeup(uint64_t) {}
static inline void esp_deep_sleep_start(void) {}
static inline esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause(void) {
  return ESP_SLEEP_WAKEUP_TIMER;
}

// EXT1 wake polarity (esp_sleep.h in IDF v5). The simulator never enters a
// real sleep, so arming and isolating are no-ops.
typedef enum {
  ESP_EXT1_WAKEUP_ALL_LOW = 0,
  ESP_EXT1_WAKEUP_ANY_HIGH = 1,
  ESP_EXT1_WAKEUP_ANY_LOW = 2,  // IDF v5.x name for the ANY_LOW trigger
} esp_sleep_ext1_wakeup_mode_t;

static inline void esp_sleep_config_gpio_isolate(void) {}
