#include "engine.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include <stdlib.h>

#ifndef ENABLE_HOT_RELOAD
#include "game.h"
#include "game_state.h"

// TODO: (ARC) Honestly may not need this? We can probably just send plugin_state (once plugin api is done).
// In case of hot reloading, this persists across reload boundaries.
// In "normal" builds (where game is statically linked), it serves as a place to hold the active data for the simulation.
static GameState *gameState = NULL;
#endif

static PlatformWindow *mainWindow = NULL;
static PlatformRenderer *mainRenderer = NULL;

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
  gameState = malloc(sizeof(GameState));
  if (!gameState)  {
    Platform_DestroyRenderer(mainRenderer);
    Platform_DestroyWindow(mainWindow);
    return false;
  }

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

#ifndef ENABLE_HOT_RELOAD
  if (gameState) {
    Game_Shutdown(gameState);
    free(gameState);
  }
#endif
}
