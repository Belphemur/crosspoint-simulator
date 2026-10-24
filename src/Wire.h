#pragma once

// Simulator stub for Arduino Wire.h (I2C master).
// Provides the minimal API the firmware codebase uses:
//   TwoWire Wire; Wire.begin(); Wire.beginTransmission(); Wire.write();
//   Wire.endTransmission(); Wire.requestFrom(); Wire.available(); Wire.read().
// All operations are no-ops; this exists so BoardConfig/M5Pm1.h,
// BatteryMonitor, and other I2C-dependent modules compile on native.

#include <stdint.h>
#include <stddef.h>

class TwoWire {
 public:
  void begin() {}
  void begin(int) {}
  void begin(int, int) {}

  void beginTransmission(uint8_t) {}
  void beginTransmission(int) {}
  void write(const uint8_t *, size_t) {}
  void write(uint8_t) {}
  uint8_t endTransmission() { return 0; }
  uint8_t endTransmission(uint8_t) { return 0; }

  uint8_t requestFrom(uint8_t, uint8_t) { return 0; }
  uint8_t requestFrom(uint8_t, uint8_t, uint8_t) { return 0; }
  int available() { return 0; }
  int read() { return 0; }
};

extern TwoWire Wire;
