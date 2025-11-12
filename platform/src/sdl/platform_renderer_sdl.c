// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform_renderer.h"
#include "platform.h"
#include "platform_sdl_internal.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

PlatformRenderer *Platform_CreateRenderer(PlatformWindow *window) {
  PlatformRenderer *renderer = calloc(1, sizeof(PlatformRenderer));

  SDL_Window *sdl_window = Platform_GetNativeWindowHandle(window);

  renderer->sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
  if (!renderer->sdl_renderer) {
    free(renderer);
    return NULL;
  }

  return renderer;
}

void Platform_DestroyRenderer(PlatformRenderer *renderer) {
  if (renderer) {
    if (renderer->sdl_renderer) {
      SDL_DestroyRenderer(renderer->sdl_renderer);
    }
    free(renderer);
  }
}

void Platform_RendererClear(const PlatformRenderer *renderer) {
  SDL_RenderClear(renderer->sdl_renderer);
}

void Platform_RendererPresent(const PlatformRenderer *renderer) {
  SDL_RenderPresent(renderer->sdl_renderer);
}

void Platform_RendererSetVSync(const PlatformRenderer *renderer, int32_t vsync) {
  SDL_SetRenderVSync(renderer->sdl_renderer, vsync);
}

bool Platform_RendererGetVSync(const PlatformRenderer *renderer, int32_t *vsync) {
  return SDL_GetRenderVSync(renderer->sdl_renderer, vsync);
}

void Platform_SetRenderLogicalPresentation(const PlatformRenderer *renderer, const int32_t w, const int32_t h) {
  SDL_SetRenderLogicalPresentation(renderer->sdl_renderer, w, h, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
}

