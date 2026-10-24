#include "Arduino.h"
#include "ESPmDNS.h"
#include "WiFi.h"

class SPIClass {};
class FS {};
class HalStorage {};

ESPMock ESP;
HWCDC Serial;
// BoardConfig::serialTransport() names Serial0 on UART0-bridged boards
// (Sticky); the simulator routes every serial endpoint to the same stderr
// HWCDC implementation.
HWCDC Serial0;
SPIClass SPI;
FS SD;
HalStorage Storage;
WiFiClass WiFi;
MDNSClass MDNS;
