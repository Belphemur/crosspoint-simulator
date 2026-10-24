#pragma once

// Simulator stub for ESP-IDF's driver/rtc_io.h. PowerManager toggles the
// RTC-domain pulls on wake buttons; the host has no RTC domain, so these are
// no-ops with the real signatures.
inline void rtc_gpio_pullup_en(int) {}
inline void rtc_gpio_pullup_dis(int) {}
inline void rtc_gpio_pulldown_en(int) {}
inline void rtc_gpio_pulldown_dis(int) {}
