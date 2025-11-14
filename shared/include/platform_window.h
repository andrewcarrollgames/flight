// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef FLIGHT_PLATFORM_WINDOW_H
#define FLIGHT_PLATFORM_WINDOW_H

#include "platform_api_types.h"
#include "platform_api_enums.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

PlatformWindow *Platform_CreateWindow(const char *title, int32_t width, int32_t height, PlatformRendererType rendererType);
void Platform_DestroyWindow(PlatformWindow *window);
bool Platform_GetWindowSize(const PlatformWindow *window, int32_t *width, int32_t *height);
void Platform_SetWindowFullscreen(const PlatformWindow *window, bool fullscreen);
void Platform_SetWindowBordered(const PlatformWindow *window, bool bordered);
void Platform_SetWindowResizeable(const PlatformWindow *window, bool resizeable);
void Platform_SetWindowSurfaceVSync(const PlatformWindow *window, int32_t vsync);
bool Platform_GetWindowSurfaceVSync(const PlatformWindow *window, int32_t *vsync);

#ifdef __cplusplus
}
#endif

#endif
