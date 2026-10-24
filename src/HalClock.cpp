#include "HalClock.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

HalClock halClock;

void HalClock::begin() {
#if defined(SIMULATOR_DEVICE_X3) || defined(SIMULATOR_DEVICE_X4_PRO) ||        \
    defined(SIMULATOR_DEVICE_X4_CLASSIC) ||                                    \
    defined(SIMULATOR_DEVICE_STICKY) || defined(SIMULATOR_DEVICE_PAPERMONO) || \
    defined(SIMULATOR_DEVICE_METALIO_EINK4) ||                                 \
    defined(SIMULATOR_DEVICE_EEGO_A4) || defined(SIMULATOR_DEVICE_PICCO)
  _available = true;
#else
  _available = false;
#endif
}

void HalClock::setTimezone(const char *posixTz) {
  setenv("TZ", posixTz && posixTz[0] != '\0' ? posixTz : "UTC0", 1);
  tzset();
}

bool HalClock::localTime(struct tm &out) const {
  if (!_available)
    return false;

  const std::time_t now = std::time(nullptr);
#if defined(_WIN32)
  return localtime_s(&out, &now) == 0;
#else
  return localtime_r(&now, &out) != nullptr;
#endif
}

bool HalClock::getTime(uint8_t &hour, uint8_t &minute) const {
  if (!_available)
    return false;

  std::tm local{};
  if (!localTime(local))
    return false;
  hour = static_cast<uint8_t>(local.tm_hour);
  minute = static_cast<uint8_t>(local.tm_min);
  return true;
}

bool HalClock::getDateTime(uint16_t &year, uint8_t &month, uint8_t &day,
                           uint8_t &hour, uint8_t &minute) const {
  if (!_available)
    return false;

  std::tm local{};
  if (!localTime(local))
    return false;
  year = static_cast<uint16_t>(local.tm_year + 1900);
  month = static_cast<uint8_t>(local.tm_mon + 1);
  day = static_cast<uint8_t>(local.tm_mday);
  hour = static_cast<uint8_t>(local.tm_hour);
  minute = static_cast<uint8_t>(local.tm_min);
  return true;
}

bool HalClock::formatTime(char *buf, size_t bufSize, int offsetMinutes,
                          bool use12Hour) const {
  if (bufSize < (use12Hour ? 9u : 6u))
    return false;

  uint8_t hour;
  uint8_t minute;
  if (!getTime(hour, minute))
    return false;

  // Clamp offset to [-720, +840] (UTC-12:00 .. UTC+14:00), matching firmware docs.
  if (offsetMinutes > 840)
    offsetMinutes = 840;
  if (offsetMinutes < -720)
    offsetMinutes = -720;

  int totalMinutes = static_cast<int>(hour) * 60 + static_cast<int>(minute) +
                     offsetMinutes;
  totalMinutes = ((totalMinutes % 1440) + 1440) % 1440;

  const int hour24 = totalMinutes / 60;
  const int min = totalMinutes % 60;
  if (use12Hour) {
    const bool pm = hour24 >= 12;
    int hour12 = hour24 % 12;
    if (hour12 == 0)
      hour12 = 12;
    std::snprintf(buf, bufSize, "%d:%02d %s", hour12, min, pm ? "PM" : "AM");
  } else {
    std::snprintf(buf, bufSize, "%02d:%02d", hour24, min);
  }
  return true;
}

bool HalClock::syncFromNTP() { return _available; }
