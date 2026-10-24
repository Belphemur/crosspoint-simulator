#pragma once
#include <cstdint>
#include <mutex>

// Minimal FreeRTOS queue shim for native (simulator) builds.
// The SDK's InputManager.h includes freertos/queue.h. These are stubs.

typedef void *QueueHandle_t;
typedef int BaseType_t;

inline QueueHandle_t xQueueCreate(uint32_t /*length*/, uint32_t /*itemSize*/) {
  static int fakeQueues[64];
  static int idx = 0;
  return idx < 64 ? &fakeQueues[idx++] : nullptr;
}
inline void vQueueDelete(QueueHandle_t) {}
inline BaseType_t xQueueSend(QueueHandle_t, const void *, uint32_t) { return 1; }
inline BaseType_t xQueueSendFromISR(QueueHandle_t, const void *, void *) { return 1; }
inline BaseType_t xQueueReceive(QueueHandle_t, void *, uint32_t) { return 0; }
inline BaseType_t xQueueReceiveFromISR(QueueHandle_t, void *, void *) { return 0; }
inline uint32_t xQueueAwait(QueueHandle_t, void *, uint32_t) { return 0; }
inline uint32_t uxQueueMessagesWaiting(QueueHandle_t) { return 0; }
