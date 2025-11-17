// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform.h"

#include "platform_api.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include <SDL3/SDL.h>
#include <stdarg.h>

static Arena *g_platform_root_arena = NULL;

// The actual PlatformAPI instance with all function pointers
static PlatformAPI g_platform_api = {
    .Log = Platform_Log,
    .LogError = Platform_LogError,
    .LogWarning = Platform_LogWarning,
    .GetTicksNS = Platform_GetTicksNS,

    .CreateWindow = Platform_CreateWindow,
    .DestroyWindow = Platform_DestroyWindow,
    .GetWindowSize = Platform_GetWindowSize,
    .SetWindowFullscreen = Platform_SetWindowFullscreen,
    .SetWindowBordered = Platform_SetWindowBordered,
    .SetWindowResizeable = Platform_SetWindowResizeable,
    .SetWindowSurfaceVSync = Platform_SetWindowSurfaceVSync,
    .GetWindowSurfaceVSync = Platform_GetWindowSurfaceVSync,

    .CreateRenderer = Platform_CreateRenderer,
    .DestroyRenderer = Platform_DestroyRenderer,
    .RendererClear = Platform_RendererClear,
    .RendererPresent = Platform_RendererPresent,
    .RendererSetVSync = Platform_RendererSetVSync,
    .RendererGetVSync = Platform_RendererGetVSync,
    .SetRenderLogicalPresentation = Platform_SetRenderLogicalPresentation,

    // Arena functions
    .GetRootArena = Platform_GetRootArena,
    .ArenaCreateBump = Arena_CreateBump,
    .ArenaCreateStack = Arena_CreateStack,
    .ArenaCreateBlock = Arena_CreateBlock,
    .ArenaCreateMultiPool = Arena_CreateMultiPool,
    .ArenaCreateScratch = Arena_CreateScratch,
    .ArenaDestroy = Arena_Destroy,
    .ArenaAlloc = Arena_Alloc,
    .ArenaAllocAligned = Arena_AllocAligned,
    .ArenaReset = Arena_Reset,
    .ArenaGetUsed = Arena_GetUsed,
    .ArenaGetPeakUsed = Arena_GetPeakUsed,
    .ArenaGetCapacity = Arena_GetCapacity,
    .ArenaSetDebugName = Arena_SetDebugName,
    .ArenaMark = Arena_Mark,
    .ArenaPopTo = Arena_PopTo,
    .ArenaBeginTemp = Arena_BeginTemp,
    .ArenaEndTemp = Arena_EndTemp,
};

PlatformAPI *Platform_GetAPI(void) {
  return &g_platform_api;
}

Arena *Platform_GetRootArena(void) {
  return g_platform_root_arena;
}

bool Platform_Init(void) {
  // Create the one and only root arena
  g_platform_root_arena = Arena_CreateVirtual(GIGABYTES(4), MEGABYTES(64));
  if (!g_platform_root_arena) {
    Platform_LogError("Failed to create root arena!");
    return false;
  }

  Arena_SetDebugName(g_platform_root_arena, "Platform Root");
  Platform_Log("Platform initialized - root arena created");

  return true;
}

void Platform_Shutdown(void) {
  if (g_platform_root_arena) {
    Platform_Log("Platform shutting down - destroying root arena");
    Arena_Destroy(g_platform_root_arena);
    g_platform_root_arena = NULL;
  }
}

void Platform_Log(const char *fmt, ...) {
#ifdef FLIGHT_ENABLE_LOGGING
  va_list args;
  va_start(args, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, args);
  va_end(args);
#endif
}

void Platform_LogError(const char *fmt, ...) {
#ifdef FLIGHT_ENABLE_LOGGING
  va_list args;
  va_start(args, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
  va_end(args);
#endif
}
void Platform_LogWarning(const char *fmt, ...) {
#ifdef FLIGHT_ENABLE_LOGGING
  va_list args;
  va_start(args, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, fmt, args);
  va_end(args);
#endif
}

const char *Platform_GetBasePath(void) {
  return SDL_GetBasePath();
}

const char *Platform_GetPrefPath(const char *org, const char *app) {
  return SDL_GetPrefPath(org, app);
}

uint64_t Platform_GetTicksNS(void) {
  return SDL_GetTicksNS();
}
