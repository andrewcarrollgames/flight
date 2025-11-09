#ifndef FLIGHT_PLATFORM_WINDOW_H
#define FLIGHT_PLATFORM_WINDOW_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct PlatformWindow PlatformWindow;

PlatformWindow *Platform_CreateWindow(const char *title, int width, int height);
void Platform_DestroyWindow(PlatformWindow *window);
bool Platform_GetWindowSize(const PlatformWindow *window, int *width, int *height);

#ifdef __cplusplus
}
#endif

#endif
