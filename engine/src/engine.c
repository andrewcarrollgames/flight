// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "engine.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include <stdlib.h>

#ifndef ENABLE_HOT_RELOAD
#include "game.h"

// In case of hot reloading, this persists across reload boundaries.
// In "normal" builds (where game is statically linked), it serves as a place to hold the active data for the simulation.
// Note: this is allocated/freed in "game" code, not anywhere else.
static void *gameState = NULL;
#endif

bool Engine_Initialize(void) {
  Platform_Log("Engine Initializing.");

#ifndef ENABLE_HOT_RELOAD
  if (!Game_Initialize(&gameState)) {
    return false;
  }

  if (!gameState) {
    return false;
  }
#endif

  return true;
}

void Engine_Update(float deltaTime) {
#ifndef ENABLE_HOT_RELOAD
  Game_Update(gameState, deltaTime);
#endif
}

void Engine_Render(void) {
#ifndef ENABLE_HOT_RELOAD
  Game_Render(gameState);
#endif
}

void Engine_Shutdown(void) {
  Platform_Log("Engine Shutting Down.");
#ifndef ENABLE_HOT_RELOAD
  if (gameState) {
    Game_Shutdown(&gameState);
  }
#endif
}
