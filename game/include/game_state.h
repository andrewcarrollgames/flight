// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stdint.h>

// forward declarations
typedef struct PlatformWindow PlatformWindow;
typedef struct PlatformRenderer PlatformRenderer;
typedef struct Arena Arena;

typedef struct GameState {
  Arena* arena;           // Game's persistent arena
  Arena* frame_arena;     // Reset every frame

  PlatformWindow *window;
  PlatformRenderer *renderer;

  // FPS variables
  uint32_t numUpdates;
  float accumulatedSeconds;
  float fps;
  float fpsUpdateFrequency;
  bool enableFPS;

  // True if this should be updating right now
  bool isRunning;
} GameState;

#endif