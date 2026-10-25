#pragma once

// File-open flag constants shared by HalStorage and the firmware libraries.
//
// This must mirror SdFat's common/FsApiConstants.h NON-fcntl branch
// (USE_FCNTL_H=0, which is what SdFatConfig.h picks on a native x86_64 host).
// Firmware translation units that include SdFat headers first see SdFat's
// typedef/defines and never re-process this header; TUs that see this header
// first must end up with byte-identical types and values, or HalStorage's
// open() overloads mangle differently per TU and the link breaks — and the
// flag VALUES the firmware passes must mean the same thing inside HalStorage.
//
// Keep in sync with .pio/libdeps/*/SdFat/src/common/FsApiConstants.h.

#include <cstdint>

#ifndef O_RDONLY
#define O_RDONLY 0X00
#endif
#ifndef O_WRONLY
#define O_WRONLY 0X01
#endif
#ifndef O_RDWR
#define O_RDWR 0X02
#endif
#ifndef O_AT_END
#define O_AT_END 0X04
#endif
#ifndef O_APPEND
#define O_APPEND 0X08
#endif
#ifndef O_CREAT
#define O_CREAT 0x10
#endif
#ifndef O_TRUNC
#define O_TRUNC 0x20
#endif
#ifndef O_EXCL
#define O_EXCL 0x40
#endif
#ifndef O_SYNC
#define O_SYNC 0x80
#endif
#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

typedef uint8_t oflag_t;

#ifndef O_READ
#define O_READ O_RDONLY
#endif
#ifndef O_WRITE
#define O_WRITE O_WRONLY
#endif
