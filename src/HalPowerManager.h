#pragma once

#include <Arduino.h>
#include <InputManager.h>
#include <Logging.h>
#include <freertos/semphr.h>

#include <cassert>
#include <atomic>

#include "HalGPIO.h"

class HalPowerManager;
extern HalPowerManager powerManager; // Singleton

class HalPowerManager {
  int normalFreq = 0; // MHz
  bool isLowPower = false;

  std::atomic<unsigned long> lastNormalMs{0};
  enum LockMode { None, NormalSpeed };
  LockMode currentLockMode = None;
  SemaphoreHandle_t modeMutex = nullptr; // Protect access to currentLockMode

public:
  static constexpr int LOW_POWER_FREQ = 10; // MHz
  static constexpr unsigned long IDLE_POWER_SAVING_MS = 3000;
  static constexpr unsigned long IDLE_DOWNCLOCK_MS = 500;
  static constexpr unsigned long IDLE_LIGHT_SLEEP_MS = 1000;
  static constexpr unsigned long BATTERY_POLL_MS = 1500;
  static constexpr unsigned long LIGHT_SLEEP_SLICE_MS = 50;
  static constexpr unsigned long BUSY_SLEEP_SLICE_MS = 20;
  // EPD idle POF threshold: powers off the panel booster between idle refreshes.
  static constexpr unsigned long EPD_IDLE_POF_MS = 30UL * 1000UL;  // 30 s
  static constexpr unsigned long GT911_IDLE_SLEEP_MS = 5UL * 60UL * 1000UL;
  static constexpr unsigned long GT911_WAKE_RETRY_MS = 1000UL;
  static constexpr unsigned long GT911_PARK_RETRY_MS = 30UL * 1000UL;
  static constexpr unsigned long NORMAL_POWER_DWELL_MS = 2000;

  void begin();

  // Control CPU frequency for power saving
  void setPowerSaving(bool enabled);

  bool lightSleep(const HalGPIO &) const {
    delay(50);
    return true;
  }
  bool onEinkBusyWaitSlice(int8_t, uint8_t) { return false; }
  void noteMainLoopIteration() {}
  void noteRenderWaitBegin() {}
  void noteRenderWaitEnd() {}

  void pokeNormalSpeed() {
    lastNormalMs.store(millis(), std::memory_order_relaxed);
  }

  // Setup wake up GPIO and enter deep sleep. When autoPowerOffTimerUs is
  // non-zero an RTC timer is armed so the device wakes after that many
  // microseconds of dwell (auto power off).
  void startDeepSleep(HalGPIO &gpio, uint64_t autoPowerOffTimerUs = 0,
                      bool wakeOnHeldButton = false) const;

  // Final software power-off: drives the master rail latches LOW, disarms any
  // RTC timer and re-enters deep sleep waking only on the power button.
  [[noreturn]] void enterPowerOffSleep(HalGPIO &gpio);

  // True when the battery is on external power / charging.
  bool isBatteryCharging() const;

  // Battery health gauge read results.
  enum class BatteryHealthState : uint8_t {
    HEALTHY = 0,
    STALE = 1,
    UNSUPPORTED = 2
  };
  BatteryHealthState getBatteryHealthState() const;
  bool isBatteryHealthStale() const;

  // Shutdown reason marker (RTC-persisted across deep-sleep restarts).
  enum class ShutdownKind : uint8_t { None = 0, User = 1, AutoOff = 2 };
  static ShutdownKind takeLastShutdownKind();
  static void stageAutoPowerOff();
  static void stageUserPowerOff();
  static void clearShutdownMarker();

#if FREEINK_DEVICE_PICCO
  bool holdWhileUsbPowered(HalGPIO &gpio,
                           void (*onStatus)(bool charging,
                                            uint16_t percent)) const;
#endif

  // Get battery percentage (range 0-100)
  uint16_t getBatteryPercentage() const;

  // RAII helper class to manage power saving locks
  // Usage: create an instance of Lock in a scope to disable power saving, for
  // example when running a task that needs full performance. When the Lock
  // instance is destroyed (goes out of scope), power saving will be re-enabled.
  class Lock {
    friend class HalPowerManager;
    bool valid = false;

  public:
    explicit Lock();
    ~Lock();

    // Non-copyable and non-movable
    Lock(const Lock &) = delete;
    Lock &operator=(const Lock &) = delete;
    Lock(Lock &&) = delete;
    Lock &operator=(Lock &&) = delete;
  };
};
