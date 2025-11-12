// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform.h"
#include <SDL3/SDL.h>
#include <stdarg.h>

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

const char *Platform_GetBasePath() {
  return SDL_GetBasePath();
}

const char *Platform_GetPrefPath(const char *org, const char *app) {
  return SDL_GetPrefPath(org, app);
}

uint64_t Platform_GetTicksNS() {
  return SDL_GetTicksNS();
}
