#ifndef FLIGHT_PLATFORM_RENDERER_H
#define FLIGHT_PLATFORM_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct PlatformWindow PlatformWindow;
typedef struct PlatformRenderer PlatformRenderer;

PlatformRenderer *Platform_CreateRenderer(PlatformWindow *window);
void Platform_DestroyRenderer(PlatformRenderer *renderer);
void Platform_RendererClear(const PlatformRenderer *renderer);
void Platform_RendererPresent(const PlatformRenderer *renderer);

#ifdef __cplusplus
}
#endif

#endif /* FLIGHT_PLATFORM_RENDERER_H */
