// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef ARENA_TYPES_H
#define ARENA_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
typedef struct Arena Arena;

// Arena Types
typedef enum ArenaType {
  ARENA_TYPE_VIRTUAL,    // OS-backed virtual memory (root only)
  ARENA_TYPE_BUMP,       // Linear: append-only, reset to free all
  ARENA_TYPE_STACK,      // Stack: push/pop with markers
  ARENA_TYPE_BLOCK,      // Pool: fixed-size blocks with free-list
  ARENA_TYPE_MULTI_POOL, // Multiple pools (power-of-2 sizes)
  ARENA_TYPE_SCRATCH,    // Temporary scoped allocations
} ArenaType;

// Per-Type Arena Data
typedef struct BumpArenaData {
  size_t offset; // Current allocation offset from base
} BumpArenaData;

typedef struct StackArenaData {
  size_t offset;    // Current top of stack
  size_t peak_used; // Peak usage (for debugging)
} StackArenaData;

typedef struct BlockArenaData {
  size_t block_size;  // Size of each block
  size_t block_count; // Total number of blocks
  void *free_list;    // Head of free list
  size_t free_count;  // Available blocks
} BlockArenaData;

typedef struct MultiPoolArenaData {
  Arena *pools[8];        // One arena per size class
  size_t size_classes[8]; // {16, 32, 64, 128, 256, 512, 1024, 2048}
} MultiPoolArenaData;

typedef struct VirtualArenaData {
  size_t reserve_size;       // Total address space reserved
  size_t commit_size;        // Currently committed
  size_t page_size;          // OS page size
  size_t commit_granularity; // Commit in chunks
} VirtualArenaData;

typedef struct ScratchArenaData {
  size_t offset;      // Current offset
  uint32_t thread_id; // Owning thread
  bool auto_reset;    // Auto-reset on scope exit
} ScratchArenaData;

// Helper Types
typedef struct ArenaMarker {
  Arena *arena;
  size_t offset;
} ArenaMarker;

typedef struct ArenaTemp {
  Arena *arena;
  ArenaMarker marker;
} ArenaTemp;

#ifdef __cplusplus
}
#endif

#endif // ARENA_TYPES_H