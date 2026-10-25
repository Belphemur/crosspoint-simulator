#pragma once
class BatteryMonitor {
public:
  static constexpr int8_t PIN_NONE = -1;

  struct Status {
    bool supported = false;
    bool percentageKnown = false;
    bool millivoltsKnown = false;
    bool chargingKnown = false;
    bool externalPowerKnown = false;
    uint16_t percentage = 0;
    uint16_t millivolts = 0;
    bool charging = false;
    bool externalPower = false;
    int32_t pm1VinMv = -1;
    int32_t pm1VinOutMv = -1;
    int16_t pm1PowerSource = -1;
  };

  BatteryMonitor() {}
  BatteryMonitor(int pin, int factor = -1) {}
  void begin() {}
  static bool loadDesignCapacity() { return false; }
  int getVoltage() { return 4200; }
  int getPercentage() { return 100; }

  // The simulator has no battery telemetry path: report an unsupported
  // battery so callers keep their fallback behaviors.
  Status readStatus() const { return Status{}; }
};
