// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define PLUGIN_EXPORT __declspec(dllexport)
#else
#define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

typedef struct PluginAPI {
  uint32_t version;
  const char *name;

  // init is responsible for allocating and initializing its own state memory (state**).
  bool (*init)(void **state);

  // updates the plugin simulation given state and time since last update (in seconds)
  void (*update)(void *state, float deltaTime);

  // render is responsible for drawing the plugin
  void (*render)(void *state);

  // shutdown is responsible for cleanly shutting down and freeing its own state memory (state**).
  void (*shutdown)(void **state);
} PluginAPI;

PLUGIN_EXPORT PluginAPI *get_plugin_api(void);

#ifdef __cplusplus
}
#endif

#endif
