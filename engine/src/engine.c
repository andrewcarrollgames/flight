#include "engine.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include <stddef.h>

static PlatformWindow *mainWindow = NULL;
static PlatformRenderer *mainRenderer = NULL;

bool Engine_Initialize(void) {
  Platform_Log("Engine Initializing.");
  bool result = false;
  mainWindow = Platform_CreateWindow("flight", 800, 600);
  if (mainWindow) {
    mainRenderer = Platform_CreateRenderer(mainWindow);
    if (mainRenderer) {
      result = true;
    } else {
      Platform_DestroyWindow(mainWindow);
      mainWindow = NULL;
    }
  }

  return result;
}

void Engine_Update(void) {
  Platform_Log("Engine Updating.");
}

void Engine_Render(void) {
  Platform_Log("Engine Rendering.");

  Platform_RendererClear(mainRenderer);
  Platform_RendererPresent(mainRenderer);
}

void Engine_Shutdown(void) {
  Platform_Log("Engine Shutting Down.");

  if (mainRenderer) {
    Platform_DestroyRenderer(mainRenderer);
    mainRenderer = NULL;
  }

  if (mainWindow) {
    Platform_DestroyWindow(mainWindow);
    mainWindow = NULL;
  }
}
