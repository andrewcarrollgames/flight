#ifndef FLIGHT_PLATFORM_SDL_INTERNAL_H
#define FLIGHT_PLATFORM_SDL_INTERNAL_H

#include <SDL3/SDL.h>

/* Struct definitions - shared across SDL implementation files */
typedef struct PlatformWindow {
  SDL_Window *sdl_window;
} PlatformWindow;

typedef struct PlatformRenderer {
  SDL_Renderer *sdl_renderer;
} PlatformRenderer;

/* Internal helper functions */
SDL_Window *platform_window_get_native_handle(PlatformWindow *window);

#endif
