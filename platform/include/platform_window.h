#ifndef FLIGHT_PLATFORM_WINDOW_H
#define FLIGHT_PLATFORM_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct PlatformWindow PlatformWindow;

PlatformWindow *Platform_CreateWindow(const char *title, int width, int height);
void Platform_DestroyWindow(PlatformWindow *window);
void Platform_GetWindowSize(const PlatformWindow *window, int *width, int *height);

#ifdef __cplusplus
}
#endif

#endif