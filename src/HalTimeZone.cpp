#include "HalTimeZone.h"

namespace freeink {

TimeZoneInfo detectTimeZoneFromIp() {
  // No network in the simulator — return an invalid result.
  return TimeZoneInfo{};
}

bool resolveUtcOffsetMinutes(const char * /*ianaId*/,
                             int64_t /*utcEpochSeconds*/,
                             int &offsetMin) {
  offsetMin = 0;
  return false;
}

bool resolveZoneAbbreviation(const char * /*ianaId*/,
                             int64_t /*utcEpochSeconds*/,
                             char *out, size_t outSize) {
  if (outSize > 0)
    out[0] = '\0';
  return false;
}

}  // namespace freeink
