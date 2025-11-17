// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef PLATFORM_MEMORY_H
#define PLATFORM_MEMORY_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlatformMemoryStats {
  size_t page_size;              // OS page size (typically 4KB)
  size_t allocation_granularity; // Allocation granularity (64KB on Windows)
  size_t total_physical;         // Total physical RAM
  size_t available_physical;     // Available physical RAM
} PlatformMemoryStats;

// Reserve virtual address space (doesn't consume physical memory)
void *Platform_MemoryReserve(size_t size);

// Commit physical memory to reserved region
bool Platform_MemoryCommit(void *ptr, size_t size);

// Decommit physical memory (keeps reservation)
void Platform_MemoryDecommit(void *ptr, size_t size);

// Release entire reservation
void Platform_MemoryRelease(void *ptr, size_t size);

// Query system memory info
PlatformMemoryStats Platform_MemoryGetStats(void);

#ifdef __cplusplus
}
#endif

#endif
