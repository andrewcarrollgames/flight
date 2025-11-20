// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "game.h"

#include "arena.h"
#include "game_context.h"
#include "game_state.h"
#include "plugin_api.h"

// Not sure if this is what we want, but works for now.
#include "test_extension_api.h"


bool Game_Initialize(void **state, PlatformAPI *platformAPI, EngineAPI *engineAPI) {
  g_game_plugin.platform = platformAPI;
  g_game_plugin.engine = engineAPI;

  // Get root arena from platform
  Arena *root = PLATFORM_GET_ROOT_ARENA();
  if (!root) {
    PLATFORM_LOG_ERROR("Game_Initialize: Failed to get root arena");
    return false;
  }

  // Create game's persistent arena
  Arena *game_arena = ARENA_CREATE_BUMP(root, MEGABYTES(256), DEFAULT_ALIGNMENT);
  if (!game_arena) {
    PLATFORM_LOG_ERROR("Game_Initialize: Failed to create game arena");
    return false;
  }
  ARENA_SET_DEBUG_NAME(game_arena, "Game");

  // Allocate game state from game arena
  GameState *gameState = ARENA_ALLOC_ALIGNED(game_arena, sizeof(GameState), DEFAULT_ALIGNMENT);
  if (!gameState) {
    PLATFORM_LOG_ERROR("Game_Initialize: Failed to allocate GameState");
    ARENA_DESTROY(game_arena);
    return false;
  }

  *state = gameState;
  gameState->arena = game_arena;
  gameState->isRunning = true;

  const int32_t logicalPresentationWidth = 640;
  const int32_t logicalPresentationHeight = 360;

  // Create window
  gameState->window = PLATFORM_CREATE_WINDOW("flight",
                                             logicalPresentationWidth,
                                             logicalPresentationHeight,
                                             PLATFORM_RENDERER_OPENGL);
  if (!gameState->window) {
    PLATFORM_LOG_ERROR("Game_Initialize: Window creation failed!");
    ARENA_DESTROY(game_arena);
    return false;
  }

  PLATFORM_SET_WINDOW_FULLSCREEN(gameState->window, false);
  PLATFORM_SET_WINDOW_BORDERED(gameState->window, true);

  // Create renderer
  gameState->renderer = PLATFORM_CREATE_RENDERER(gameState->window);
  if (!gameState->renderer) {
    PLATFORM_DESTROY_WINDOW(gameState->window);
    gameState->window = NULL;
    ARENA_DESTROY(game_arena);
    return false;
  }

  PLATFORM_SET_RENDER_LOGICAL_PRESENTATION(gameState->renderer,
                                           logicalPresentationWidth,
                                           logicalPresentationHeight);

  // Create subsystem arenas
  gameState->frame_arena = ARENA_CREATE_STACK(game_arena, MEGABYTES(4), DEFAULT_ALIGNMENT);
  ARENA_SET_DEBUG_NAME(gameState->frame_arena, "Game::Frame");

  // FPS tracking
  gameState->enableFPS = true;
  gameState->fps = 0.0f;
  gameState->accumulatedSeconds = 0.0f;
  gameState->fpsUpdateFrequency = 2.0f;
  gameState->numUpdates = 0;

  PLATFORM_LOG("Game initialized with arena system");
  PLATFORM_LOG("  Game arena: %zu MB allocated",
               ARENA_GET_CAPACITY(game_arena) / (1024 * 1024));

  return true;
}

void Game_Update(void *state, const float deltaTime) {
  GameState *gameState = (GameState *)(state);
  if (!gameState || !gameState->isRunning)
    return;

  // Reset frame arena at start of each frame
  ARENA_RESET(gameState->frame_arena);

  // Test out an engine extension.
  static bool tested = false; // persists locally (this logic will only run on the first update)
  if (!tested) {
    TEST_LOG_HELLO();
    tested = true;
  }

  // FPS tracking
  gameState->accumulatedSeconds += deltaTime;
  ++gameState->numUpdates;

  if (gameState->accumulatedSeconds > gameState->fpsUpdateFrequency) {
    gameState->fps = (float)gameState->numUpdates / gameState->accumulatedSeconds;

    PLATFORM_LOG("Game fps: (%.2f), dt: (%.6f), # updates: (%llu), elapsed time since last print: (%.6f seconds)", gameState->fps, deltaTime, gameState->numUpdates, gameState->accumulatedSeconds);
    PLATFORM_LOG("  Game arena used: %zu / %zu bytes (%.1f%%)",
                 ARENA_GET_USED(gameState->arena),
                 ARENA_GET_CAPACITY(gameState->arena),
                 100.0f * ARENA_GET_USED(gameState->arena) / ARENA_GET_CAPACITY(gameState->arena));
    PLATFORM_LOG("  Frame arena used: %zu / %zu bytes",
                 ARENA_GET_USED(gameState->frame_arena),
                 ARENA_GET_CAPACITY(gameState->frame_arena));

    gameState->numUpdates = 0;
    gameState->accumulatedSeconds = 0.0f;
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

    // Destroy game arena (frees all game memory including GameState)
    // Note: frame_arena is a child, so it gets destroyed automatically
    if (gameState->arena) {
      ARENA_DESTROY(gameState->arena);
    }

    gameState->isRunning = false;
    *state = NULL;
  }
}

PLUGIN_EXPORT PluginAPI *GetPluginAPI(void) {
  return &g_game_plugin;
}
