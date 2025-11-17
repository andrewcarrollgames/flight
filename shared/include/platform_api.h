// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef PLATFORM_API_H
#define PLATFORM_API_H

#include "arena.h"
#include "platform_api_enums.h"
#include "platform_api_types.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Platform API - All platform services available to plugins
typedef struct PlatformAPI {
  // Logging
  void (*Log)(const char *fmt, ...);
  void (*LogError)(const char *fmt, ...);
  void (*LogWarning)(const char *fmt, ...);

  // Timing
  uint64_t (*GetTicksNS)(void);

  // Window Management
  PlatformWindow *(*CreateWindow)(const char *title, int32_t width, int32_t height, PlatformRendererType type);
  void (*DestroyWindow)(PlatformWindow *window);
  bool (*GetWindowSize)(const PlatformWindow *window, int32_t *width, int32_t *height);
  void (*SetWindowFullscreen)(const PlatformWindow *window, bool fullscreen);
  void (*SetWindowBordered)(const PlatformWindow *window, bool bordered);
  void (*SetWindowResizeable)(const PlatformWindow *window, bool resizeable);
  void (*SetWindowSurfaceVSync)(const PlatformWindow *window, int32_t vsync);
  bool (*GetWindowSurfaceVSync)(const PlatformWindow *window, int32_t *vsync);

  // Renderer Management
  PlatformRenderer *(*CreateRenderer)(PlatformWindow *window);
  void (*DestroyRenderer)(PlatformRenderer *renderer);
  void (*RendererClear)(const PlatformRenderer *renderer);
  void (*RendererPresent)(const PlatformRenderer *renderer);
  void (*RendererSetVSync)(const PlatformRenderer *renderer, int32_t vsync);
  bool (*RendererGetVSync)(const PlatformRenderer *renderer, int32_t *vsync);
  void (*SetRenderLogicalPresentation)(const PlatformRenderer *renderer, int32_t w, int32_t h);

  // Memory / Arena Management
  Arena *(*GetRootArena)(void);
  Arena *(*ArenaCreateBump)(Arena *parent, size_t size, size_t alignment);
  Arena *(*ArenaCreateStack)(Arena *parent, size_t size, size_t alignment);
  Arena *(*ArenaCreateBlock)(Arena *parent, size_t block_size, size_t count, size_t alignment);
  Arena *(*ArenaCreateMultiPool)(Arena *parent, size_t total_size);
  Arena *(*ArenaCreateScratch)(Arena *parent, size_t size, size_t alignment);
  void (*ArenaDestroy)(Arena *arena);
  void *(*ArenaAlloc)(Arena *arena, size_t size);
  void *(*ArenaAllocAligned)(Arena *arena, size_t size, size_t alignment);
  void (*ArenaReset)(Arena *arena);
  size_t (*ArenaGetUsed)(Arena *arena);
  size_t (*ArenaGetPeakUsed)(Arena *arena);
  size_t (*ArenaGetCapacity)(Arena *arena);
  void (*ArenaSetDebugName)(Arena *arena, const char *name);
  ArenaMarker (*ArenaMark)(Arena *arena);
  void (*ArenaPopTo)(Arena *arena, ArenaMarker marker);
  ArenaTemp (*ArenaBeginTemp)(Arena *arena);
  void (*ArenaEndTemp)(ArenaTemp temp);
} PlatformAPI;

// Getter for platform API (implemented by platform layer)
PlatformAPI *Platform_GetAPI(void);

#ifdef __cplusplus
}
#endif

#endif
