// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "arena.h"
#include "platform.h"
#include "platform_memory.h"
#include <stdint.h>
#include <string.h>

// ============================================================================
// Internal Helpers
// ============================================================================
static void *align_pointer(void *ptr, size_t alignment) {
  uintptr_t addr = (uintptr_t)ptr;
  uintptr_t aligned = ALIGN_UP(addr, alignment);
  return (void *)aligned;
}

static size_t align_size(size_t size, size_t alignment) {
  return ALIGN_UP(size, alignment);
}

static void link_child_to_parent(Arena *child, Arena *parent) {
  if (!parent)
    return;

  child->parent = parent;
  child->next_sibling = parent->first_child;
  parent->first_child = child;
}

static void unlink_child_from_parent(Arena *child) {
  if (!child->parent)
    return;

  Arena *parent = child->parent;

  // Remove from parent's child list
  if (parent->first_child == child) {
    parent->first_child = child->next_sibling;
  } else {
    Arena *prev = parent->first_child;
    while (prev && prev->next_sibling != child) {
      prev = prev->next_sibling;
    }
    if (prev) {
      prev->next_sibling = child->next_sibling;
    }
  }

  child->parent = NULL;
  child->next_sibling = NULL;
}

// ============================================================================
// Virtual Arena (Root Only)
// ============================================================================
Arena *Arena_CreateVirtual(size_t reserve_size, size_t commit_size) {
  PlatformMemoryStats stats = Platform_MemoryGetStats();

  // Round reserve size to allocation granularity
  reserve_size = align_size(reserve_size, stats.allocation_granularity);

  // Commit size defaults to one granularity chunk if not specified
  if (commit_size == 0) {
    commit_size = stats.allocation_granularity;
  }
  commit_size = align_size(commit_size, stats.page_size);

  // Reserve virtual address space
  void *base = Platform_MemoryReserve(reserve_size);
  if (!base) {
    Platform_LogError("Failed to reserve %zu bytes for virtual arena", reserve_size);
    return NULL;
  }

  // Commit initial memory
  if (!Platform_MemoryCommit(base, commit_size)) {
    Platform_LogError("Failed to commit %zu bytes for virtual arena", commit_size);
    Platform_MemoryRelease(base, reserve_size);
    return NULL;
  }

  // Place Arena struct at the beginning of committed memory
  Arena *arena = (Arena *)base;
  memset(arena, 0, sizeof(Arena));

  arena->type = ARENA_TYPE_VIRTUAL;
  arena->base = (char *)base + sizeof(Arena);
  arena->raw_base = base;
  arena->size = reserve_size - sizeof(Arena);
  arena->used = 0;
  arena->peak_used = 0;
  arena->alignment = DEFAULT_ALIGNMENT;
  arena->parent = NULL;
  arena->first_child = NULL;
  arena->next_sibling = NULL;

  // Virtual arena specific data
  arena->data.virtual_mem.reserve_size = reserve_size;
  arena->data.virtual_mem.commit_size = commit_size - sizeof(Arena);
  arena->data.virtual_mem.page_size = stats.page_size;
  arena->data.virtual_mem.commit_granularity = stats.allocation_granularity;

  Platform_Log("Virtual arena created: %zu MB reserved, %zu KB committed",
               reserve_size / (1024 * 1024),
               commit_size / 1024);

  return arena;
}

// ============================================================================
// Bump Arena (Linear Allocator)
// ============================================================================
Arena *Arena_CreateBump(Arena *parent, size_t size, size_t alignment) {
  if (!parent) {
    Platform_LogError("Bump arena requires a parent. Use Platform_GetRootArena()");
    return NULL;
  }

  // Allocate space for Arena struct + user data + alignment padding
  size_t total_size = sizeof(Arena) + size + alignment;
  void *raw_mem = Arena_Alloc(parent, total_size);
  if (!raw_mem) {
    Platform_LogError("Failed to allocate %zu bytes from parent for bump arena", total_size);
    return NULL;
  }

  // Arena struct at the beginning
  Arena *arena = (Arena *)raw_mem;
  memset(arena, 0, sizeof(Arena));

  // Align base pointer after Arena struct
  void *user_base = (char *)raw_mem + sizeof(Arena);
  void *aligned_base = align_pointer(user_base, alignment);

  arena->type = ARENA_TYPE_BUMP;
  arena->base = aligned_base;
  arena->raw_base = raw_mem;
  arena->size = size;
  arena->used = 0;
  arena->peak_used = 0;
  arena->alignment = alignment;

  // Link to parent
  link_child_to_parent(arena, parent);

  // Bump-specific data
  arena->data.bump.offset = 0;

  return arena;
}

// ============================================================================
// Stack Arena (Push/Pop with markers)
// ============================================================================
Arena *Arena_CreateStack(Arena *parent, size_t size, size_t alignment) {
  if (!parent) {
    Platform_LogError("Stack arena requires a parent. Use Platform_GetRootArena()");
    return NULL;
  }

  // Allocate space for Arena struct + user data + alignment padding
  size_t total_size = sizeof(Arena) + size + alignment;
  void *raw_mem = Arena_Alloc(parent, total_size);
  if (!raw_mem) {
    Platform_LogError("Failed to allocate %zu bytes from parent for stack arena", total_size);
    return NULL;
  }

  // Arena struct at the beginning
  Arena *arena = (Arena *)raw_mem;
  memset(arena, 0, sizeof(Arena));

  // Align base pointer after Arena struct
  void *user_base = (char *)raw_mem + sizeof(Arena);
  void *aligned_base = align_pointer(user_base, alignment);

  arena->type = ARENA_TYPE_STACK;
  arena->base = aligned_base;
  arena->raw_base = raw_mem;
  arena->size = size;
  arena->used = 0;
  arena->peak_used = 0;
  arena->alignment = alignment;

  // Link to parent
  link_child_to_parent(arena, parent);

  // Stack-specific data
  arena->data.stack.offset = 0;
  arena->data.stack.peak_used = 0;

  return arena;
}

// ============================================================================
// Block Arena (TODO: Implement)
// ============================================================================
Arena *Arena_CreateBlock(Arena *parent, size_t block_size, size_t block_count, size_t alignment) {
  Platform_LogError("Arena_CreateBlock not yet implemented");
  return NULL;
}

// ============================================================================
// Multi-Pool Arena (TODO: Implement)
// ============================================================================
Arena *Arena_CreateMultiPool(Arena *parent, size_t total_size) {
  Platform_LogError("Arena_CreateMultiPool not yet implemented");
  return NULL;
}

// ============================================================================
// Scratch Arena (TODO: Implement)
// ============================================================================
Arena *Arena_CreateScratch(Arena *parent, size_t size, size_t alignment) {
  Platform_LogError("Arena_CreateScratch not yet implemented");
  return NULL;
}

// ============================================================================
// Arena Destruction
// ============================================================================
void Arena_Destroy(Arena *arena) {
  if (!arena)
    return;

  // Recursively destroy all children first
  Arena *child = arena->first_child;
  while (child) {
    Arena *next = child->next_sibling;
    Arena_Destroy(child);
    child = next;
  }

  // Unlink from parent
  unlink_child_from_parent(arena);

  // Free based on type
  if (arena->type == ARENA_TYPE_VIRTUAL) {
    // Virtual arena owns OS memory
    Platform_MemoryRelease(arena->raw_base, arena->data.virtual_mem.reserve_size);
    Platform_Log("Virtual arena destroyed");
  }
  // Child arenas don't free anything - parent owns their memory
}

// ============================================================================
// Arena Allocation
// ============================================================================
void *Arena_AllocAligned(Arena *arena, size_t size, size_t alignment) {
  if (!arena)
    return NULL;
  if (size == 0)
    return NULL;

  void *result = NULL;

  switch (arena->type) {
    case ARENA_TYPE_VIRTUAL: {
      // Virtual arena allocates from its own memory
      size_t current_offset = arena->used;
      uintptr_t current_ptr = (uintptr_t)arena->base + current_offset;
      uintptr_t aligned_ptr = ALIGN_UP(current_ptr, alignment);
      size_t padding = aligned_ptr - current_ptr;
      size_t total_size = padding + size;

      // Check if we need to commit more memory
      size_t new_used = arena->used + total_size;
      if (new_used > arena->data.virtual_mem.commit_size) {
        // Need to commit more pages
        size_t commit_size = arena->data.virtual_mem.commit_size;
        size_t needed = new_used - commit_size;
        size_t to_commit = align_size(needed, arena->data.virtual_mem.page_size);

        void *commit_ptr = (char *)arena->base + commit_size;
        if (!Platform_MemoryCommit(commit_ptr, to_commit)) {
          Platform_LogError("Failed to commit %zu more bytes to virtual arena", to_commit);
          return NULL;
        }

        arena->data.virtual_mem.commit_size += to_commit;
        Platform_Log("Virtual arena grew: committed %zu KB more", to_commit / 1024);
      }

      if (new_used > arena->size) {
        Platform_LogError("Virtual arena out of address space (%zu / %zu bytes used)",
                          new_used, arena->size);
        return NULL;
      }

      result = (void *)aligned_ptr;
      arena->used = new_used;

      break;
    }

    case ARENA_TYPE_BUMP: {
      // Bump arena allocates linearly
      size_t current_offset = arena->data.bump.offset;
      uintptr_t current_ptr = (uintptr_t)arena->base + current_offset;
      uintptr_t aligned_ptr = ALIGN_UP(current_ptr, alignment);
      size_t padding = aligned_ptr - current_ptr;
      size_t total_size = padding + size;

      if (arena->data.bump.offset + total_size > arena->size) {
        Platform_LogError("Bump arena out of memory (%zu / %zu bytes used)",
                          arena->data.bump.offset + total_size, arena->size);
        return NULL;
      }

      result = (void *)aligned_ptr;
      arena->data.bump.offset += total_size;
      arena->used = arena->data.bump.offset;

      break;
    }
    case ARENA_TYPE_STACK: {
      // Stack arena allocates just like Bump
      size_t current_offset = arena->data.stack.offset;
      uintptr_t current_ptr = (uintptr_t)arena->base + current_offset;
      uintptr_t aligned_ptr = ALIGN_UP(current_ptr, alignment);
      size_t padding = aligned_ptr - current_ptr;
      size_t total_size = padding + size;

      if (arena->data.stack.offset + total_size > arena->size) {
        Platform_LogError("Stack arena out of memory (%zu / %zu bytes used)",
                          arena->data.stack.offset + total_size, arena->size);
        return NULL;
      }

      result = (void *)aligned_ptr;
      arena->data.stack.offset += total_size;
      arena->used = arena->data.stack.offset;

      // Track peak for stack specifically
      if (arena->data.stack.offset > arena->data.stack.peak_used) {
        arena->data.stack.peak_used = arena->data.stack.offset;
      }

      break;
    }
    default:
      Platform_LogError("Arena type %d not yet implemented for allocation", arena->type);
      return NULL;
  }

  // Update peak usage
  if (arena->used > arena->peak_used) {
    arena->peak_used = arena->used;
  }

  return result;
}

void *Arena_Alloc(Arena *arena, size_t size) {
  return Arena_AllocAligned(arena, size, arena ? arena->alignment : DEFAULT_ALIGNMENT);
}

// ============================================================================
// Arena Reset
// ============================================================================
void Arena_Reset(Arena *arena) {
  if (!arena)
    return;

  switch (arena->type) {
    case ARENA_TYPE_BUMP:
      arena->data.bump.offset = 0;
      arena->used = 0;
      break;

    case ARENA_TYPE_VIRTUAL:
      // Don't reset virtual arena - it's the root!
      Platform_LogWarning("Cannot reset virtual arena - it's the root allocator");
      break;

    case ARENA_TYPE_STACK:
      arena->data.stack.offset = 0;
      arena->used = 0;
      // Note: We keep peak_used for statistics
      break;

    default:
      Platform_LogError("Arena type %d not yet implemented for reset", arena->type);
      break;
  }
}

// ============================================================================
// Arena Query Functions
// ============================================================================
size_t Arena_GetUsed(Arena *arena) {
  return arena ? arena->used : 0;
}

size_t Arena_GetPeakUsed(Arena *arena) {
  return arena ? arena->peak_used : 0;
}

size_t Arena_GetCapacity(Arena *arena) {
  return arena ? arena->size : 0;
}

void Arena_SetDebugName(Arena *arena, const char *name) {
  if (arena) {
    arena->debug_name = name;
  }
}

// ============================================================================
// Stack Arena Specific Operations (Update existing stubs)
// ============================================================================

ArenaMarker Arena_Mark(Arena *arena) {
  ArenaMarker marker = {0};
  if (!arena)
    return marker;

  if (arena->type == ARENA_TYPE_STACK) {
    marker.arena = arena;
    marker.offset = arena->data.stack.offset;
  } else {
    Platform_LogError("Arena_Mark only valid for ARENA_TYPE_STACK");
  }

  return marker;
}

void Arena_PopTo(Arena *arena, ArenaMarker marker) {
  if (!arena || arena != marker.arena)
    return;

  if (arena->type == ARENA_TYPE_STACK) {
    // Validate the marker is from this arena's past
    if (marker.offset > arena->data.stack.offset) {
      Platform_LogError("Invalid marker: trying to pop to future offset (%zu > %zu)",
                       marker.offset, arena->data.stack.offset);
      return;
    }

    // Pop back to the marked position
    arena->data.stack.offset = marker.offset;
    arena->used = marker.offset;
  } else {
    Platform_LogError("Arena_PopTo only valid for ARENA_TYPE_STACK");
  }
}

// ============================================================================
// Scoped Temporary Allocations (Update existing functions)
// ============================================================================

ArenaTemp Arena_BeginTemp(Arena *arena) {
  ArenaTemp temp = {0};
  if (!arena) return temp;

  temp.arena = arena;

  // Works with both Stack and Bump arenas
  if (arena->type == ARENA_TYPE_STACK) {
    temp.marker = Arena_Mark(arena);
  } else if (arena->type == ARENA_TYPE_BUMP) {
    // For Bump, we can fake it by saving the offset
    temp.marker.arena = arena;
    temp.marker.offset = arena->data.bump.offset;
  } else {
    Platform_LogError("Arena_BeginTemp only works with STACK or BUMP arenas");
  }

  return temp;
}

void Arena_EndTemp(ArenaTemp temp) {
  if (!temp.arena) return;

  if (temp.arena->type == ARENA_TYPE_STACK) {
    Arena_PopTo(temp.arena, temp.marker);
  } else if (temp.arena->type == ARENA_TYPE_BUMP) {
    // For Bump, restore the offset
    temp.arena->data.bump.offset = temp.marker.offset;
    temp.arena->used = temp.marker.offset;
  }
}
