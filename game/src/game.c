// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "game.h"
#include "game_state.h"
#include "platform.h"
#include "platform_renderer.h"
#include "platform_window.h"
#include "plugin_api.h"
#include <stdlib.h>

static PluginAPI g_game_plugin = {
    .version = 1,
    .name = "Flight Game",
    .init = Game_Initialize,
    .update = Game_Update,
    .render = Game_Render,
    .shutdown = Game_Shutdown};

bool Game_Initialize(void **state) {
  // TODO: (ARC): Use Arenas from shared/platform for this allocation.
  GameState *gameState = (GameState *)malloc(sizeof(GameState));
  *state = gameState;
  if (!gameState) {
    Platform_LogError("Game_Initialize: Failed to allocate GameState");
    return false;
  }

  gameState->isRunning = true;
  const int32_t logicalPresentationWidth = 640;
  const int32_t logicalPresentationHeight = 360;

  // TODO: (ARC) A default value could be defined in our config?
  gameState->window = Platform_CreateWindow("flight", logicalPresentationWidth, logicalPresentationHeight, PLATFORM_RENDERER_OPENGL);
  if (!gameState->window) {
    Platform_LogError("Game_Initialize: Window creation failed!");
    free(gameState);
    return false;
  }

  Platform_SetWindowFullscreen(gameState->window, true);
  Platform_SetWindowBordered(gameState->window, false);
  // Platform_SetWindowResizeable(gameState->window, false);
  // Platform_SetWindowSurfaceVSync(gameState->window, 1);

  gameState->renderer = Platform_CreateRenderer(gameState->window);
  if (!gameState->renderer) {
    Platform_DestroyWindow(gameState->window);
    gameState->window = NULL;
    return false;
  }

  Platform_SetRenderLogicalPresentation(gameState->renderer, logicalPresentationWidth, logicalPresentationHeight);
  // Platform_RendererSetVSync(gameState->renderer, 1);

  gameState->enableFPS = true;
  gameState->fps = 0.0f;
  gameState->accumulatedSeconds = 0.0f;
  gameState->fpsUpdateFrequency = 2.0f;
  gameState->numUpdates = 0;

  return true;
}

void Game_Update(void *state, const float deltaTime) {
  GameState *gameState = (GameState *)(state);
  if (gameState && gameState->isRunning) {
    gameState->accumulatedSeconds += deltaTime;
    ++gameState->numUpdates;
    if (gameState->accumulatedSeconds > gameState->fpsUpdateFrequency) {
      gameState->fps = (float)gameState->numUpdates / gameState->accumulatedSeconds;
      Platform_Log("Game fps: (%.2f), dt: (%.6f), NumUpdates: (%d), accumulatedSeconds: (%.5f)", gameState->fps, deltaTime, gameState->numUpdates, gameState->accumulatedSeconds);
      gameState->numUpdates = 0;
      gameState->accumulatedSeconds = 0.0f;
    }
  }
}

void Game_Render(void *state) {
  const GameState *gameState = (GameState *)state;
  Platform_RendererClear(gameState->renderer);
  Platform_RendererPresent(gameState->renderer);
}

void Game_Shutdown(void **state) {
  GameState *gameState = (GameState *)(*state);

  if (gameState) {
    if (gameState->renderer) {
      Platform_DestroyRenderer(gameState->renderer);
      gameState->renderer = NULL;
    }

    if (gameState->window) {
      Platform_DestroyWindow(gameState->window);
      gameState->window = NULL;
    }

    Platform_Log("Game Shutting Down.");
    gameState->isRunning = false;

    // TODO: (ARC): Use Arenas from shared/platform for this free.
    free(gameState);
    *state = NULL;
  }
}

PLUGIN_EXPORT PluginAPI *GetPluginAPI(void) {
  return &g_game_plugin;
}
