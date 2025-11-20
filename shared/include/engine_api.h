// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef ENGINE_API_H
#define ENGINE_API_H

#include "engine_api_enums.h"
#include "engine_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Engine API - All engine core and extension services available to plugins
typedef struct EngineAPI {
  void* (*GetExtensionAPI)(const char* name);
} EngineAPI;

// Getter for engine API (implemented by engine layer)
EngineAPI *Engine_GetAPI(void);

#ifdef __cplusplus
}
#endif

#endif
