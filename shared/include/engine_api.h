// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef ENGINE_API_H
#define ENGINE_API_H

#include "engine_api_enums.h"
#include "engine_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Engine API - All engine services available to plugins
typedef struct EngineAPI {
  // ECS (placeholder - add when you implement it)
  // Entity* (*CreateEntity)(void);
  // void (*DestroyEntity)(Entity* entity);

  // For now, empty struct needs at least one member
  int _placeholder;
} EngineAPI;

// Getter for engine API (implemented by engine layer)
EngineAPI *Engine_GetAPI(void);

#ifdef __cplusplus
}
#endif

#endif
