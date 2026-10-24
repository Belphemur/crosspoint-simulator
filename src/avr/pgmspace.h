#pragma once

// Simulator stub for AVR's avr/pgmspace.h. Some Arduino libraries (AceTime)
// fall back to this include on "untested platforms" — the host build is one.
// Flash is RAM here, so every _P function is its plain libc counterpart.
// This is the SINGLE home for the pgmspace shims: Arduino.h and Print.h
// include it rather than redefining the functions (ODR-safe).
#include <cstddef>
#include <string.h>

// Storage-class no-op: flash is RAM on the host.
#ifndef PROGMEM
#define PROGMEM
#endif

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_ptr(addr) (*(const void *const *)(addr))

inline int strcmp_P(const char *a, const char *b) { return strcmp(a, b); }
inline int strncmp_P(const char *a, const char *b, size_t n) {
  return strncmp(a, b, n);
}
inline char *strchr_P(const char *s, int c) { return strchr(s, c); }
inline char *strrchr_P(const char *s, int c) { return strrchr(s, c); }
inline size_t strlen_P(const char *s) { return strlen(s); }
inline char *strcpy_P(char *dest, const char *src) { return strcpy(dest, src); }
inline char *strncpy_P(char *dest, const char *src, size_t n) {
  return strncpy(dest, src, n);
}
inline char *strncat_P(char *dest, const char *src, size_t n) {
  return strncat(dest, src, n);
}
inline void *memcpy_P(void *dest, const void *src, size_t n) {
  return memcpy(dest, src, n);
}
inline int memcmp_P(const void *a, const void *b, size_t n) {
  return memcmp(a, b, n);
}
inline char *strcat_P(char *dest, const char *src) { return strcat(dest, src); }
