// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "game.h"
#include "game_state.h"
#include "plugin_api.h"
#include <stdlib.h>

bool Game_Initialize(void **state, PlatformAPI *platformAPI, EngineAPI *engineAPI) {
  // TODO: (ARC): Use Arenas from shared/platform for this allocation.
  GameState *gameState = (GameState *)malloc(sizeof(GameState));
  *state = gameState;

  g_game_plugin.platform = platformAPI;
  g_game_plugin.engine = engineAPI;

  if (!gameState) {
    PLATFORM_LOG("Game_Initialize: Failed to allocate GameState");
    return false;
  }

  gameState->isRunning = true;
  const int32_t logicalPresentationWidth = 640;
  const int32_t logicalPresentationHeight = 360;

  // TODO: (ARC) A default value could be defined in our config?
  gameState->window = PLATFORM_CREATE_WINDOW("flight", logicalPresentationWidth, logicalPresentationHeight, PLATFORM_RENDERER_OPENGL);
  if (!gameState->window) {
    PLATFORM_LOG_ERROR("Game_Initialize: Window creation failed!");
    free(gameState);
    return false;
  }

  PLATFORM_SET_WINDOW_FULLSCREEN(gameState->window, true);
  PLATFORM_SET_WINDOW_BORDERED(gameState->window, false);
  // Platform_SetWindowResizeable(gameState->window, false);
  // Platform_SetWindowSurfaceVSync(gameState->window, 1);

  gameState->renderer = PLATFORM_CREATE_RENDERER(gameState->window);
  if (!gameState->renderer) {
    PLATFORM_DESTROY_WINDOW(gameState->window);
    gameState->window = NULL;
    free(gameState);
    return false;
  }

  PLATFORM_SET_RENDER_LOGICAL_PRESENTATION(gameState->renderer, logicalPresentationWidth, logicalPresentationHeight);
  // Platform_RendererSetVSync(gameState->renderer, 1);

  gameState->enableFPS = true;
  gameState->fps = 0.0f;
  gameState->accumulatedSeconds = 0.0f;
  gameState->fpsUpdateFrequency = 0.5f;
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
      PLATFORM_LOG("Game fps: (%.2f), dt: (%.6f), NumUpdates: (%d), accumulatedSeconds: (%.5f)", gameState->fps, deltaTime, gameState->numUpdates, gameState->accumulatedSeconds);
      gameState->numUpdates = 0;
      gameState->accumulatedSeconds = 0.0f;
    }
  }
}

void Game_Render(void *state) {
  const GameState *gameState = (GameState *)state;
  PLATFORM_RENDERER_CLEAR(gameState->renderer);
  PLATFORM_RENDERER_PRESENT(gameState->renderer);
}

void Game_Shutdown(void **state) {
  GameState *gameState = (GameState *)(*state);

  if (gameState) {
    if (gameState->renderer) {
      PLATFORM_DESTROY_RENDERER(gameState->renderer);
      gameState->renderer = NULL;
    }

    if (gameState->window) {
      PLATFORM_DESTROY_WINDOW(gameState->window);
      gameState->window = NULL;
    }

    PLATFORM_LOG("Game Shutting Down.");
    gameState->isRunning = false;

    // TODO: (ARC): Use Arenas from shared/platform for this free.
    free(gameState);
    *state = NULL;
  }
}

PLUGIN_EXPORT PluginAPI *GetPluginAPI(void) {
  return &g_game_plugin;
}
