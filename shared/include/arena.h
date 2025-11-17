// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef ARENA_H
#define ARENA_H

#include "arena_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Arena structure
struct Arena {
  ArenaType type;

  void *base;       // Aligned base address
  void *raw_base;   // Original pointer (for freeing)
  size_t size;      // Total usable size
  size_t used;      // Currently used bytes
  size_t peak_used; // High water mark
  size_t alignment; // Default alignment

  Arena *parent;       // Parent arena (NULL for root)
  Arena *first_child;  // First child in linked list
  Arena *next_sibling; // Next sibling in parent's child list

  const char *debug_name; // For visualization/debugging

  union {
    BumpArenaData bump;
    StackArenaData stack;
    BlockArenaData block;
    MultiPoolArenaData multi_pool;
    VirtualArenaData virtual_mem;
    ScratchArenaData scratch;
  } data;
};

// ============================================================================
// Arena Creation (Virtual is internal to platform only)
// ============================================================================

// Create Virtual arena - ONLY called by Platform_Init!
// This is the root arena that all others chain from
Arena *Arena_CreateVirtual(size_t reserve_size, size_t commit_size);

// Create Bump arena (linear allocator)
Arena *Arena_CreateBump(Arena *parent, size_t size, size_t alignment);

// Create Stack arena (push/pop with markers)
Arena *Arena_CreateStack(Arena *parent, size_t size, size_t alignment);

// Create Block arena (fixed-size pool)
Arena *Arena_CreateBlock(Arena *parent, size_t block_size, size_t block_count, size_t alignment);

// Create Multi-pool arena (power-of-2 size classes)
Arena *Arena_CreateMultiPool(Arena *parent, size_t total_size);

// Create Scratch arena (temporary scoped)
Arena *Arena_CreateScratch(Arena *parent, size_t size, size_t alignment);

// ============================================================================
// Arena Operations
// ============================================================================

// Destroy arena and all children
void Arena_Destroy(Arena *arena);

// Allocate memory with default alignment
void *Arena_Alloc(Arena *arena, size_t size);

// Allocate memory with specific alignment
void *Arena_AllocAligned(Arena *arena, size_t size, size_t alignment);

// Reset arena (behavior depends on type)
void Arena_Reset(Arena *arena);

// Get current usage statistics
size_t Arena_GetUsed(Arena *arena);
size_t Arena_GetPeakUsed(Arena *arena);
size_t Arena_GetCapacity(Arena *arena);

// Set debug name for visualization
void Arena_SetDebugName(Arena *arena, const char *name);

// ============================================================================
// Stack Arena Specific Operations
// ============================================================================

// Save current position
ArenaMarker Arena_Mark(Arena *arena);

// Pop back to saved position
void Arena_PopTo(Arena *arena, ArenaMarker marker);

// ============================================================================
// Scoped Temporary Allocations
// ============================================================================

// Begin temporary allocation scope
ArenaTemp Arena_BeginTemp(Arena *arena);

// End temporary allocation scope
void Arena_EndTemp(ArenaTemp temp);

// ============================================================================
// Helper Macros
// ============================================================================

#define ALIGN_UP(ptr, align) \
  (((ptr) + ((align) - 1)) & ~((align) - 1))

#define ALIGN_DOWN(ptr, align) \
  ((ptr) & ~((align) - 1))

#define IS_ALIGNED(ptr, align) \
  (((uintptr_t)(ptr) & ((align) - 1)) == 0)

// Common sizes
#define KILOBYTES(n) ((size_t)(n) * 1024)
#define MEGABYTES(n) ((size_t)(n) * 1024 * 1024)
#define GIGABYTES(n) ((size_t)(n) * 1024 * 1024 * 1024)

// Common alignments
#define DEFAULT_ALIGNMENT 8
#define SIMD_ALIGNMENT 16
#define CACHE_LINE_SIZE 64

// Type-aware allocation
#define Arena_AllocType(arena, type) \
  (type *)Arena_AllocAligned(arena, sizeof(type), _Alignof(type))

#define Arena_AllocArray(arena, type, count) \
  (type *)Arena_AllocAligned(arena, sizeof(type) * (count), _Alignof(type))

// Scoped temp with defer-style cleanup
#define ARENA_TEMP(arena_var)                                         \
  for (ArenaTemp _temp = Arena_BeginTemp(arena_var), *_once = &_temp; \
       _once;                                                         \
       Arena_EndTemp(_temp), _once = NULL)

#ifdef __cplusplus
}
#endif

#endif