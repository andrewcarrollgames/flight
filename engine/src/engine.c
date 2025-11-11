#include "engine.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include "plugin_api.h"
#include <stdlib.h>

#ifndef ENABLE_HOT_RELOAD
#include "game.h"

// In case of hot reloading, this persists across reload boundaries.
// In "normal" builds (where game is statically linked), it serves as a place to hold the active data for the simulation.
// Note: this is allocated/freed in "game" code, not anywhere else.
static void *gameState = NULL;
#endif

static PlatformWindow *mainWindow = NULL;
static PlatformRenderer *mainRenderer = NULL;

uint64_t prevFrameTimeNS;
const float nanoSecondsToSeconds = (1.0f / 1000000000.0f);

bool Engine_Initialize(void) {
  Platform_Log("Engine Initializing.");
  mainWindow = Platform_CreateWindow("flight", 800, 600);
  if (!mainWindow) {
    return false;
  }

  mainRenderer = Platform_CreateRenderer(mainWindow);
  if (!mainRenderer) {
    Platform_DestroyWindow(mainWindow);
    mainWindow = NULL;
    return false;
  }

#ifndef ENABLE_HOT_RELOAD
  if (!Game_Initialize(&gameState)) {
    return false;
  }

  if (!gameState) {
    Platform_DestroyRenderer(mainRenderer);
    Platform_DestroyWindow(mainWindow);
    return false;
  }
#endif

  return true;
}

void Engine_Update(void) {
  const uint64_t currentTimeNS = Platform_GetTicksNS();
  const float deltaTime = (float)(currentTimeNS - prevFrameTimeNS) * nanoSecondsToSeconds;
  prevFrameTimeNS = currentTimeNS;
#ifndef ENABLE_HOT_RELOAD
  Game_Update(gameState, deltaTime);
#endif
}

void Engine_Render(void) {
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

#ifndef ENABLE_HOT_RELOAD
  if (gameState) {
    Game_Shutdown(&gameState);
  }
#endif
}
