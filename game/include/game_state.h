#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct GameState {
  uint64_t version;
  bool isRunning;
} GameState;

#endif
