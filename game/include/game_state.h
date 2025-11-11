#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

// Forward declarations of the APIs used in the game.dll when hot reload is enabled.
typedef struct EngineAPI EngineAPI;
typedef struct PlatformAPI PlatformAPI;

typedef struct GameState {
  EngineAPI* engine;
  PlatformAPI* platform;
  uint64_t version;
  bool isRunning;
} GameState;

#endif
