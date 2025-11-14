// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform.h"
#include "platform_api.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include <SDL3/SDL.h>
#include <stdarg.h>

// The actual PlatformAPI instance with all function pointers
static PlatformAPI g_platform_api = {
  .Log = Platform_Log,
  .LogError = Platform_LogError,
  .LogWarning = Platform_LogWarning,
//  .Alloc = Platform_Alloc,
//  .Free = Platform_Free,
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
};

PlatformAPI* Platform_GetAPI(void) {
  return &g_platform_api;
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
