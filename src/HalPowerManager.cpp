#include "HalPowerManager.h"

#include "HalGPIO.h"
#include "SimulatorLifecycle.h"
#include <cstdlib>

HalPowerManager powerManager;

void HalPowerManager::begin() {}
void HalPowerManager::startDeepSleep(HalGPIO &gpio,
                                     uint64_t /*autoPowerOffTimerUs*/,
                                     bool wakeOnHeldButton) const {
  // In the simulator, autoPowerOffTimerUs has no RTC timer to arm.
  // The auto power-off path is exercised at the application layer
  // (main.cpp) which calls enterPowerOffSleep() when the timer fires.
  if (wakeOnHeldButton)
    SimulatorLifecycle::rebootAsPowerWake();
  gpio.startDeepSleep();
}
void HalPowerManager::enterPowerOffSleep(HalGPIO &gpio) {
  gpio.startDeepSleep();
  std::exit(0);  // [[noreturn]] — simulator exits
}
bool HalPowerManager::isBatteryCharging() const { return true; }
HalPowerManager::BatteryHealthState
HalPowerManager::getBatteryHealthState() const {
  return BatteryHealthState::HEALTHY;
}
bool HalPowerManager::isBatteryHealthStale() const { return false; }
HalPowerManager::ShutdownKind HalPowerManager::takeLastShutdownKind() {
  return ShutdownKind::None;
}
void HalPowerManager::stageAutoPowerOff() {}
void HalPowerManager::stageUserPowerOff() {}
void HalPowerManager::clearShutdownMarker() {}
#if FREEINK_DEVICE_PICCO
bool HalPowerManager::holdWhileUsbPowered(
    HalGPIO & /*gpio*/, void (* /*onStatus*/)(bool, uint16_t)) const {
  // No charger model: keep the existing host sleep/wake path available.
  return false;
}
#endif
void HalPowerManager::setPowerSaving(bool enable) {}
uint16_t HalPowerManager::getBatteryPercentage() const { return 100; }

HalPowerManager::Lock::Lock() {}
HalPowerManager::Lock::~Lock() {}
