#include "platform_sdl_internal.h"
#include "platform_window.h"
#include <SDL3/SDL.h>

PlatformWindow *Platform_CreateWindow(const char *title, int width, int height) {
  PlatformWindow *window = SDL_calloc(1, sizeof(PlatformWindow));
  window->sdl_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
  if (!window->sdl_window) {
    SDL_free(window);
    return NULL;
  }
  return window;
}

void Platform_DestroyWindow(PlatformWindow *window) {
  if (window && window->sdl_window) {
    SDL_DestroyWindow(window->sdl_window);
  }
  free(window);
}

bool Platform_GetWindowSize(const PlatformWindow *window, int *width, int *height) {
  return SDL_GetWindowSize(window->sdl_window, width, height);
}

SDL_Window *platform_window_get_native_handle(PlatformWindow *window) { // NOLINT (matches function signature, should absolutely not be const)
  return window->sdl_window;
}
