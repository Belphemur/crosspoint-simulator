# Simulator parity — decisions log

Branch `sim-parity`. Each entry records a choice made while driving
`pio run -e simulator` (from the firmware repo) to a clean build, beyond what
`simulator-parity-design.md` specified. Committed per the job brief's rules.

## Task 2 — BoardConfig.h stub

1. **The brief's X4 `InputPins` numbers were wrong.** The brief specified
   `{back:8, confirm:10, left:21, right:4, up:5, down:6, power:-1}` — those are
   the real header's *DisplayPins* for the X4, not its buttons. The real
   `XTEINK_X4` profile carries `InputStyle::XteinkAdcLadder` with
   `InputPins {0, 1, 2, 3, 4, 5, 3, false}` (ladder taps on GPIO0-5, power on
   GPIO3, active-low). Per the standing rule ("prefer grepping the REAL header
   over guessing") the real values were used.
2. **The stub was rebuilt as a verbatim mirror of the SDK header's body**, not
   a curated expansion. The firmware's own sources compile against this stub
   too (PIO builds libraries before `src/`, and the stub wins every
   `BoardConfig.h` include via `-I` order), and they touch nearly every
   BoardProfile member (`sensors`, `batteryGauge`, `audio`, `frontlight`,
   `sdmmc`, `mic`, `power`, `haptic`, `usbDetect`, ...). A curated subset would
   have re-broken on each round. The copy carries three documented simulator
   adaptations: no ESP includes (power-latch helpers become no-ops),
   `SIMULATOR_DISPLAY_*` controller override for the X4 family profiles, and
   the X3 `DEFAULT_DEVICE` honouring `SIMULATOR_DISPLAY_UC8279`.
3. **All device profiles now carry the real header's values**, which changes
   two things vs the old stub:
   - X3 geometry is 792x528 (the old stub silently used the 800x480 defaults).
     X3 simulator runs will now size buffers to the real panel.
   - Picco's touch controller is `TouchController::Tma525c` (real header) —
     the stub's invented `PiccoCst` enumerator is gone. Grep confirmed no
     simulator source referenced `PiccoCst`.
4. **`serialTransport()`'s Sticky branch needs `Serial0`**, so
   `src/HardwareSerial.h` declares it and `ESP.cpp` defines it (same
   stderr-backed HWCDC as `Serial`, matching the stub's documented
   "single serial endpoint" intent).

## Task 5 — post-design build fixes (in fix order)

5. **`Print.h` was rewritten** to the full Arduino overload family (number
   bases, float, `String`, `__FlashStringHelper`, `write(const char*)`) once
   SdFat's debug/formatter TUs compiled for the first time; the old three-line
   stub couldn't express them.
6. **`FreeRTOS.h` includes `task.h` at the END of the file.** The ESP-IDF port's
   `FreeRTOS.h` pulls in the task API (SDCardManager relies on it), but a
   top-of-file include deadlocks the shim's definitions (task.h uses
   `BaseType_t`/`TaskHandle_t` from FreeRTOS.h). Including last makes both
   include orders work.
7. **`platformio.local.ini` build-flag additions** (the brief's sanctioned
   escape hatch; all are recorded here):
   - `-fpermissive` — SdFat's `iostream/ostream.h` casts `const void*` to
     `uint32_t` (64-bit host pointers); a real lib defect on 64-bit targets.
   - `-std=gnu11` placed BEFORE `-std=gnu++2a` — gcc 15 defaults C files to
     C23, where `bool` is a keyword and QRCode's `qrcode.c` (`typedef unsigned
     char bool;`) breaks. Ordering matters: the last `-std` wins per language,
     so C gets gnu11 and C++ keeps gnu++2a.
   - `-DUSE_WOLFSSL_IO -DNO_WRITEV` — Arduino-wolfSSL's `user_settings.h`
     enables TLS only for boards it recognises (ESP32/ESP8266/AVR...), so on
     the host `USE_WOLFSSL_IO` was never defined: `wolfio.c` skipped
     `EmbedSend`/`EmbedReceive` while `internal.c` still assigned them, and
     `ssl.c`'s `wolfSSL_writev` referenced `struct iovec`. Both defines are
     wolfSSL-namespace only.
   - `-include errno.h` — wolfSSL's `wolfio.h` uses `errno`/`EWOULDBLOCK`/...
     without including `<errno.h>` in this configuration.
   - `-DREADING_STATS_ENABLED=1` — the `[base]` section flag the simulator env
     does not inherit; without it `XtcReaderActivity::loop()` compiles out
     while the header declares it unconditionally (undefined vtable reference).
8. **`lib/MinizInflate` is fixed by a build middleware, not source edits.**
   `miniz_wrapper.c` includes FreeInkBook's `miniz.c` AND `miniz_cores.c` in
   one TU. On ESP that works because `miniz_cores.c` is empty under
   `ESP_PLATFORM` and the ROM overrides `miniz.c`'s core objects at link time;
   on a host build `miniz.c` (full impl: wrapper/zlib API + tdefl core, but NO
   tinfl core — that binds to ROM) and `miniz_cores.c` (tinfl + tdefl +
   `mz_adler32`/`mz_crc32`) conflict. The firmware repo is read-only for this
   job, so `skip_device_content_protection.py` (already registered in the sim
   env's `extra_scripts`) gained a middleware that generates, in
   `$BUILD_DIR/patched/`: a `miniz_cores_sim.c` truncated before miniz_cores'
   tdefl section (tinfl + zlib-style helpers only, ESP guard re-closed) and a
   `miniz_wrapper.c` with absolute includes. Result: the combined TU defines
   every plain-name symbol exactly once (`mz_inflate*` for ContentProtection's
   `CONTENT_EXTERNAL_MINIZ`, plain `tinfl_decompress` for lib/inflate).
9. **`oflag_t` was unified with SdFat's native ABI.** SdFat on a native x86_64
   host takes its `USE_FCNTL_H=0` branch: `typedef uint8_t oflag_t` with
   SdFat flag values (O_CREAT=0x10 etc.). The simulator's
   `common/FsApiConstants.h` had `typedef int oflag_t` with POSIX values, so
   TUs disagreed on the mangled signature of `HalStorage::open` (and, worse,
   on flag VALUES). The stub now mirrors SdFat's non-fcntl branch exactly, and
   `HalStorage::open` translates the SdFat-style bits to POSIX flags at the
   `::open(2)` boundary (the access-mode bits coincide; the modifier bits do
   not — SdFat O_CREAT=0x10 is host O_EXCL). `O_AT_END` now positions the
   handle at EOF, matching SdFat semantics used by the clippings and
   web-upload paths.
10. **The LCP link stubs were removed** from `firmware_link_stubs.cpp`
    (`parseLcpLicense`, `lcpUserKey`, `lcpUnwrapContentKey`): once the SDK's
    ContentProtection lib compiled (WolfsslCrypto.h's SIMULATOR branch), its
    `LcpLicense.o` provided the real definitions and the stubs became multiple
    definitions. `openProtectedBook` stays — the firmware's
    `lib/Epub/ContentProtection.cpp` is dropped by the skip middleware and the
    SDK lib never defines it.
11. **`JPEGDEC`/`PNGDEC` simulator decoders gained `openRAM`** (decode from an
    in-memory buffer via the existing stb-backed `SimulatorImageDecode`
    path), matching the bitbank2 API shape the cover/thumb converters use.
