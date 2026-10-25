#pragma once

// Simulator stub for ESP-IDF's esp_idf_version.h. The firmware libraries
// (e.g. BatteryMonitor) gate code on the IDF version; the simulator models
// the ESP-IDF v5.x line the firmware targets.
#define ESP_IDF_VERSION_MAJOR 5
#define ESP_IDF_VERSION_MINOR 0
#define ESP_IDF_VERSION_PATCH 0

#define ESP_IDF_VERSION_VAL(major, minor, patch)                               \
  ((major << 16) | (minor << 8) | (patch))
#define ESP_IDF_VERSION                                                        \
  ESP_IDF_VERSION_VAL(ESP_IDF_VERSION_MAJOR, ESP_IDF_VERSION_MINOR,            \
                      ESP_IDF_VERSION_PATCH)
