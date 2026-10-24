#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "WString.h"

// Some libraries (AceCommon) include <Print.h> directly and still use the
// flash-string API, which the ESP32 core exposes through Arduino.h — pull the
// single pgmspace shim home in here as well.
#include "avr/pgmspace.h"

// Arduino Print base, host stub. Carries the full overload surface the
// firmware and its third-party libraries (SdFat's debug/formatter paths,
// ArduinoJson, ...) call; all output is discarded except what a concrete
// subclass routes somewhere (e.g. HWCDC -> stderr).

class Print {
public:
  virtual ~Print() = default;
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      n += write(*buffer++);
    }
    return n;
  }
  // Arduino Print also exposes the char-based write forms SdFat's print_t
  // interface relies on (writeMsg uses pr->write("literal")).
  size_t write(const char *str) {
    return str ? write(reinterpret_cast<const uint8_t *>(str), strlen(str)) : 0;
  }
  size_t write(const char *buffer, size_t size) {
    return write(reinterpret_cast<const uint8_t *>(buffer), size);
  }
  virtual void flush() {}

  size_t print(const char *s) { return write((const uint8_t *)s, strlen(s)); }
  size_t print(const __FlashStringHelper *s) {
    // Flash is RAM on the host; the tag carries no indirection.
    return s ? print(reinterpret_cast<const char *>(s)) : 0;
  }
  size_t print(const String &s) { return print(s.c_str()); }
  size_t print(char c) { return write(static_cast<uint8_t>(c)); }
  size_t print(unsigned char n, int base = 10) {
    return printNumber(static_cast<unsigned long long>(n), base);
  }
  size_t print(int n, int base = 10) {
    return printNumber(static_cast<long long>(n), base);
  }
  size_t print(unsigned int n, int base = 10) {
    return printNumber(static_cast<unsigned long long>(n), base);
  }
  size_t print(long n, int base = 10) {
    return printNumber(static_cast<long long>(n), base);
  }
  size_t print(unsigned long n, int base = 10) {
    return printNumber(static_cast<unsigned long long>(n), base);
  }
  size_t print(long long n, int base = 10) { return printNumber(n, base); }
  size_t print(unsigned long long n, int base = 10) {
    return printNumber(n, base);
  }
  size_t print(double n, int digits = 2) { return printFloat(n, digits); }

  size_t println() { return print("\r\n"); }
  size_t println(const char *s) { return print(s) + println(); }
  size_t println(const __FlashStringHelper *s) {
    return print(s) + println();
  }
  size_t println(const String &s) { return print(s) + println(); }
  size_t println(char c) { return print(c) + println(); }
  size_t println(unsigned char n, int base = 10) {
    return print(n, base) + println();
  }
  size_t println(int n, int base = 10) { return print(n, base) + println(); }
  size_t println(unsigned int n, int base = 10) {
    return print(n, base) + println();
  }
  size_t println(long n, int base = 10) { return print(n, base) + println(); }
  size_t println(unsigned long n, int base = 10) {
    return print(n, base) + println();
  }
  size_t println(long long n, int base = 10) {
    return print(n, base) + println();
  }
  size_t println(unsigned long long n, int base = 10) {
    return print(n, base) + println();
  }
  size_t println(double n, int digits = 2) {
    return print(n, digits) + println();
  }

private:
  size_t printNumber(long long n, int base) {
    char buf[8 * sizeof(long long) + 2];
    if (base == 10) {
      int len = snprintf(buf, sizeof(buf), "%lld", n);
      return len > 0 ? write(buf, static_cast<size_t>(len)) : 0;
    }
    // Non-decimal bases render the magnitude after a minus sign, like the
    // Arduino core's printNumber.
    size_t lead = 0;
    unsigned long long v;
    if (n < 0) {
      buf[lead++] = '-';
      v = -(static_cast<unsigned long long>(n));
    } else {
      v = static_cast<unsigned long long>(n);
    }
    if (base == 2) {
      char *p = buf + sizeof(buf) - 1;
      *p = '\0';
      if (v == 0)
        *--p = '0';
      while (v) {
        *--p = static_cast<char>('0' + (v & 1));
        v >>= 1;
      }
      return write(buf, static_cast<size_t>(buf + sizeof(buf) - 1 - p));
    }
    int len = snprintf(buf + lead, sizeof(buf) - lead,
                       base == 16 ? "%llX" : "%llo", v);
    return len > 0 ? write(buf, lead + static_cast<size_t>(len)) : 0;
  }
  size_t printNumber(unsigned long long n, int base) {
    return printNumber(static_cast<long long>(n), base);
  }
  size_t printFloat(double n, int digits) {
    char buf[64];
    if (digits < 0)
      digits = 2;
    if (digits > 17)
      digits = 17;
    int len = snprintf(buf, sizeof(buf), "%.*f", digits, n);
    return len > 0 ? write(buf, static_cast<size_t>(len)) : 0;
  }
};
