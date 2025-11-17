// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct PlatformWindow PlatformWindow;
typedef struct PlatformRenderer PlatformRenderer;

typedef struct GameState {
  PlatformWindow *window;
  PlatformRenderer *renderer;

  // FPS variables.
  bool enableFPS;
} GameState;

#ifdef __cplusplus
}
#endif

#endif
