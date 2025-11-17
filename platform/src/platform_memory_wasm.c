#ifdef __EMSCRIPTEN__

#include "platform.h"
#include "platform_memory.h"
#include <emscripten.h>
#include <stdlib.h>

// WebAssembly uses a different memory model - no virtual memory
// We simulate it with malloc/free

void *Platform_MemoryReserve(size_t size) {
  // In WASM, just allocate immediately
  void *ptr = malloc(size);

  if (!ptr) {
    Platform_LogError("malloc failed for %zu bytes", size);
  }

  return ptr;
}

bool Platform_MemoryCommit(void *ptr, size_t size) {
  // In WASM, memory is already committed
  (void)ptr;
  (void)size;
  return true;
}

void Platform_MemoryDecommit(void *ptr, size_t size) {
  // No-op in WASM - can't decommit
  (void)ptr;
  (void)size;
}

void Platform_MemoryRelease(void *ptr, size_t size) {
  (void)size;
  free(ptr);
}

PlatformMemoryStats Platform_MemoryGetStats(void) {
  PlatformMemoryStats stats = {0};

  // WebAssembly page size is 64KB
  stats.page_size = 65536;
  stats.allocation_granularity = 65536;

  // Get heap size from Emscripten
  stats.total_physical = (size_t)emscripten_get_heap_size();

  // Available is harder to determine accurately in WASM
  // This is a rough estimate
  stats.available_physical = stats.total_physical / 2;

  return stats;
}

#endif