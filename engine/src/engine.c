#include "engine.h"
#include "game_state.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"

#ifndef ENABLE_HOT_RELOAD
#include "game.h"
#endif

#include <stdlib.h>

static PlatformWindow *mainWindow = NULL;
static PlatformRenderer *mainRenderer = NULL;

// In case of hot reloading, this persists across reload boundaries.
// In "normal" builds, it serves as a place to hold the active data for the simulation.
static GameState *gameState = NULL;

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

  gameState = malloc(sizeof(GameState));
  if (!gameState)  {
    Platform_DestroyRenderer(mainRenderer);
    Platform_DestroyWindow(mainWindow);
    return false;
  }

#ifndef ENABLE_HOT_RELOAD
  if (!Game_Initialize(gameState)) {
    return false;
  }
#endif

  return true;
}

void Engine_Update(void) {
  Platform_Log("Engine Updating.");
#ifndef ENABLE_HOT_RELOAD
  Game_Update(gameState);
#endif
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

  if (gameState) {
#ifndef ENABLE_HOT_RELOAD
    Game_Shutdown(gameState);
#endif
    free(gameState);
  }
}
