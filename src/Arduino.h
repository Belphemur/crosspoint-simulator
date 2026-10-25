#pragma once
#include <algorithm>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

#include "freertos/FreeRTOS.h"

#define PROGMEM
#define ICACHE_RODATA_ATTR
#define IRAM_ATTR
#define DRAM_ATTR
#define RTC_NOINIT_ATTR
#define SET_LOOP_TASK_STACK_SIZE(bytes)
#define PGM_P const char *
#define PSTR(s) (s)

inline unsigned long millis() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return duration_cast<milliseconds>(steady_clock::now() - start).count();
}

inline unsigned long micros() {
  using namespace std::chrono;
  static const auto start = steady_clock::now();
  return duration_cast<microseconds>(steady_clock::now() - start).count();
}

inline void delay(unsigned long ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
inline void yield() { std::this_thread::yield(); }
inline void configTzTime(const char *, const char *) {}

// Arduino core pin/polarity constants. The simulator has no real GPIO, but
// SDK headers (BoardConfig.h, EpdBus.h) reference these constants directly.
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif
#ifndef INPUT_PULLDOWN
#define INPUT_PULLDOWN 0x3
#endif

// SPI bus / mode constants used by SDK display drivers.
#ifndef SPI_MODE0
#define SPI_MODE0 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif
#define HSPI 1
#define VSPI 2

// Native builds have no GPIO pins. Treat every input as released, matching the
// idle pull-up state used by the button diagnostics in firmware startup.
#define CHANGE 1
// Print number bases (Arduino core constants).
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
// Pin-constant defaults from the ESP32 core's pins_arduino.h. Meaningless on
// the host, but SdFat and other libraries name them as default arguments.
// (SDA/SCL are NOT defined here: board headers define them as constexpr ints.)
#define SS 5
#define MOSI 23
#define MISO 19
#define SCK 18
inline int digitalRead(int /*pin*/) { return 1; }
inline void pinMode(int /*pin*/, int /*mode*/) {}
inline void digitalWrite(int /*pin*/, int /*level*/) {}
inline void analogWrite(int /*pin*/, int /*val*/) {}
// Native builds have no ADC. Reads return 0, matching a ladder input that
// never reaches a press threshold. The attenuation/calibration surface lives
// in esp_adc_cal.h, which the Arduino-ESP32 core exposes through this header.
#include "esp_adc_cal.h"
inline int analogRead(uint8_t /*pin*/) { return 0; }
inline void delayMicroseconds(uint32_t /*us*/) {}
// Interrupt attachments are host no-ops: nothing on the simulator raises a
// GPIO edge. The BUSY-line ISR paths (EpdBus) compile but never fire.
using esp_interrupt_f = void (*)();
inline void attachInterrupt(int /*pin*/, void (* /*isr*/)(), int /*mode*/) {}
inline void detachInterrupt(int /*pin*/) {}
inline int digitalPinToInterrupt(int pin) { return pin; }

// Flash-memory read shims (ESP32's pgmspace.h). On the host, flash == RAM.
// All definitions live in avr/pgmspace.h (the compatibility include some
// libraries hard-code); this header just pulls them in.
#include "avr/pgmspace.h"

// ESP-IDF FreeRTOS port stubs used by firmware source code.
inline uint32_t xPortGetCoreID() { return 0; }

#include "HardwareSerial.h"
#include "Print.h"
#include "WString.h"

struct ESPMock {
  static constexpr uint32_t HEAP_SIZE = 1024 * 1024;

  static uint32_t heapValue(const char *name) {
    const char *value = std::getenv(name);
    if (!value || *value == '\0')
      return HEAP_SIZE;

    char *end = nullptr;
    errno = 0;
    const unsigned long parsed = std::strtoul(value, &end, 10);
    if (errno == ERANGE || *end != '\0' || parsed > HEAP_SIZE)
      return HEAP_SIZE;
    return static_cast<uint32_t>(parsed);
  }

  uint32_t getFreeHeap() { return heapValue("CROSSPOINT_SIM_FREE_HEAP"); }
  void restart() {}
  uint32_t getHeapSize() { return HEAP_SIZE; }
  uint32_t getMinFreeHeap() { return getFreeHeap(); }
  uint32_t getMaxAllocHeap() {
    return std::min(heapValue("CROSSPOINT_SIM_MAX_ALLOC_HEAP"), getFreeHeap());
  }
  uint32_t getFreePsram() { return 0; }  // No PSRAM in host builds
  const char *getChipModel() const { return "simulator"; }
  uint32_t getChipRevision() const { return 0; }
  uint32_t getFlashChipSize() const { return 16U * 1024U * 1024U; }
};
extern ESPMock ESP;

// ESP-IDF makes the heap-caps API ubiquitous; the sim's host shim mirrors it
// so BOARD_HAS_PSRAM code paths (PSRAM-backed stores, HomeActivity's cover
// buffer) compile and call straight into std::malloc with an 8 MB accounting.
// Placed after `extern ESPMock ESP;` because the shim's heap accounting
// reads ESP.getFreeHeap().
#include "esp_heap_caps.h"

// Arduino-ESP32 core's PSRAM API (esp32-hal-psram.h). The host sim "has"
// the 8 MB PSRAM X4 Pro boards report in simulatorPsramBytes(); the small
// non-PSRAM devices still run this code unconditionally, where psramFound()
// mirrors the real hardware (no PSRAM on the plain X4).
inline bool psramFound() { return simulatorPsramBytes() > 0; }
inline bool psramInit() { return psramFound(); }
inline void psramAddToHeap() {}
inline size_t psramGetRemainingSize() { return simulatorPsramBytes(); }

inline long random(long max) { return std::rand() % max; }

template <typename A, typename B>
constexpr auto max(A a, B b) -> decltype(a > b ? a : b) {
  return a > b ? a : b;
}
template <typename A, typename B>
constexpr auto min(A a, B b) -> decltype(a < b ? a : b) {
  return a < b ? a : b;
}
