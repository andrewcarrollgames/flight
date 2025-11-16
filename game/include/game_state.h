#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

// forward declarations
typedef struct PlatformWindow PlatformWindow;
typedef struct PlatformRenderer PlatformRenderer;

typedef struct GameState {
  PlatformWindow *window;
  PlatformRenderer *renderer;

  // FPS variables.
  bool enableFPS;
} GameState;

#endif
