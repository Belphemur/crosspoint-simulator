#pragma once
#include <cstdint>

// Minimal SPI shim for native (simulator) builds. The SDK's display drivers
// (EpdBus, It8951Driver) and the firmware include <SPI.h> and call these APIs.
// These are no-ops / passthroughs — no physical bus exists on the host.

class SPISettings {
public:
  SPISettings(uint32_t /*clock*/, uint8_t /*bitOrder*/, uint8_t /*mode*/) {}
  SPISettings() {}
};

class SPIClass {
public:
  void begin(int sck = -1, int miso = -1, int mosi = -1, int ss = -1) {}
  void end() {}
  void beginTransaction(SPISettings /*settings*/) {}
  void endTransaction() {}
  uint8_t transfer(uint8_t data) { return data; }
  void transfer(uint8_t *buf, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) buf[i] = transfer(buf[i]);
  }
  void write(uint8_t data) { (void)data; }
  void write(const uint8_t *buf, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) write(buf[i]);
  }
  void writeBytes(const uint8_t *buf, uint32_t size) { write(buf, size); }
};

extern SPIClass SPI;
