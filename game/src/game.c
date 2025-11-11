#include "game.h"
#include "game_state.h"
#include "platform.h"
#include <stdlib.h>

uint32_t numUpdates = 0;
float accumulatedSeconds = 0.0f;
float fps = 0.0f;
const float fpsUpdateFrequency = 2.0f;

bool Game_Initialize(void **state) {
  // TODO: (ARC): Use Arenas from shared/platform for this allocation.
  GameState* gameState = (GameState*)malloc(sizeof(GameState));
  *state = gameState;

  if (gameState) {
    Platform_Log("Game Initializing.");
    gameState->isRunning = true;
  }
  return true;
}

void Game_Update(void *state, const float delta_time) {
  GameState* gameState = (GameState*)(state);
  if (gameState && gameState->isRunning) {
    accumulatedSeconds += delta_time;
    ++numUpdates;
    if (accumulatedSeconds > fpsUpdateFrequency) {
      fps = (float)numUpdates/accumulatedSeconds;
      Platform_Log("Game fps: (%.2f), dt: (%.5f), NumUpdates: (%d), accumulatedSeconds: (%.5f)", fps, delta_time, numUpdates, accumulatedSeconds);
      numUpdates = 0;
      accumulatedSeconds = 0.0f;
    }
  }
}

void Game_Shutdown(void **state) {
  GameState* gameState = (GameState*)(*state);
  if (gameState) {
    Platform_Log("Game Shutting Down.");
    gameState->isRunning = false;
    // TODO: (ARC): Use Arenas from shared/platform for this free.
    free(gameState);
    *state = NULL;
  }
}
