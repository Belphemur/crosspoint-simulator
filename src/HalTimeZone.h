#pragma once

#include <cstddef>
#include <cstdint>

namespace freeink {

struct TimeZoneInfo {
  bool valid = false;
  char id[40] = "";
  int offsetMin = 0;
  bool isDst = false;
};

// Best-effort time zone detection via IP geolocation.
// In the simulator there is no network, so this returns an invalid result.
TimeZoneInfo detectTimeZoneFromIp();

// Resolve the live UTC offset (minutes, signed) for an IANA zone id at a given
// UTC epoch, folding DST automatically from the bundled AceTime zone database.
// Returns true and writes `offsetMin` on success.
bool resolveUtcOffsetMinutes(const char *ianaId, int64_t utcEpochSeconds,
                             int &offsetMin);

static constexpr size_t kZoneAbbrevSize = 8;

// Resolve the zone abbreviation in effect at a given UTC epoch.
// Returns false when the id is unknown/empty or the epoch is out of range.
bool resolveZoneAbbreviation(const char *ianaId, int64_t utcEpochSeconds,
                             char *out, size_t outSize);

}  // namespace freeink
