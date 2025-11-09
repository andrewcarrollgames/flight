#include "platform_renderer.h"
#include "platform_sdl_internal.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

PlatformRenderer *Platform_CreateRenderer(PlatformWindow *window) {
  PlatformRenderer *renderer = calloc(1, sizeof(PlatformRenderer));

  SDL_Window *sdl_window = platform_window_get_native_handle(window);

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
