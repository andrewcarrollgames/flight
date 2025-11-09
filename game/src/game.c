#include "game.h"
#include "game_state.h"
#include "platform.h"

bool Game_Initialize(GameState *gameState) {
  Platform_Log("Game Initializing.");
  gameState->isRunning = true;
  return true;
}

void Game_Update(GameState *gameState) {
  if (gameState->isRunning) {
    Platform_Log("Game Updating.");
  }
}

void Game_Shutdown(GameState *gameState) {
  Platform_Log("Game Shutting Down.");
  gameState->isRunning = false;
}
