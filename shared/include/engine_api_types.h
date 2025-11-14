// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef ENGINE_API_TYPES_H
#define ENGINE_API_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

  // Forward declarations for engine types
  typedef struct EngineAPI EngineAPI;
  typedef struct Entity Entity;
  typedef struct Component Component;
  typedef struct Job Job;

  // Add more as you build engine systems:
  // typedef struct AssetHandle AssetHandle;
  // typedef struct Scene Scene;

#ifdef __cplusplus
}
#endif

#endif
