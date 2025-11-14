// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef FLIGHT_PLATFORM_RENDERER_H
#define FLIGHT_PLATFORM_RENDERER_H

#include "platform_api_types.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

PlatformRenderer *Platform_CreateRenderer(PlatformWindow *window);
void Platform_DestroyRenderer(PlatformRenderer *renderer);
void Platform_RendererClear(const PlatformRenderer *renderer);
void Platform_RendererPresent(const PlatformRenderer *renderer);
void Platform_RendererSetVSync(const PlatformRenderer *renderer, int32_t vsync);
bool Platform_RendererGetVSync(const PlatformRenderer *renderer, int32_t *vsync);
void Platform_SetRenderLogicalPresentation(const PlatformRenderer *renderer, int32_t w, int32_t h);

#ifdef __cplusplus
}
#endif

#endif /* FLIGHT_PLATFORM_RENDERER_H */
