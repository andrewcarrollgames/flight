// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform_sdl_internal.h"
#include "platform_window.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

PlatformWindow *Platform_CreateWindow(const char *title, const int32_t width, const int32_t height, const PlatformRendererType rendererType) {
  PlatformWindow *window = SDL_calloc(1, sizeof(PlatformWindow));
  SDL_WindowFlags windowFlags = 0;
  switch (rendererType) {
    case PLATFORM_RENDERER_VULKAN:
      windowFlags = SDL_WINDOW_VULKAN;
      break;
    case PLATFORM_RENDERER_METAL:
      windowFlags = SDL_WINDOW_METAL;
      break;
    case PLATFORM_RENDERER_OPENGL:
    default:
      windowFlags = SDL_WINDOW_OPENGL;
      break;
  }

  window->sdl_window = SDL_CreateWindow(title, width, height, windowFlags);
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

bool Platform_GetWindowSize(const PlatformWindow *window, int32_t *width, int32_t *height) {
  const SDL_Surface *windowSurface = SDL_GetWindowSurface(window->sdl_window);
  if (windowSurface) {
    *width = windowSurface->w;
    *height = windowSurface->h;
    return true;
  }
  return false;
}

SDL_Window *Platform_GetNativeWindowHandle(PlatformWindow *window) { // NOLINT (matches function signature, should absolutely not be const)
  return window->sdl_window;
}

void Platform_SetWindowFullscreen(const PlatformWindow *window, const bool fullscreen) {
  SDL_SetWindowFullscreen(window->sdl_window, fullscreen);
}

void Platform_SetWindowBordered(const PlatformWindow *window, const bool bordered) {
  SDL_SetWindowBordered(window->sdl_window, bordered);
}

void Platform_SetWindowResizeable(const PlatformWindow *window, const bool resizeable) {
  SDL_SetWindowResizable(window->sdl_window, resizeable);
}

void Platform_SetWindowSurfaceVSync(const PlatformWindow *window, const int32_t vsync) {
  SDL_SetWindowSurfaceVSync(window->sdl_window, 0);
}

bool Platform_GetWindowSurfaceVSync(const PlatformWindow *window, int32_t *vsync) {
  return SDL_GetWindowSurfaceVSync(window->sdl_window, vsync);
}