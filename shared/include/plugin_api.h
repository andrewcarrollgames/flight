#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PluginAPI {
  uint32_t version;
  const char *name;

  // init is responsible for allocating and initializing its own state memory (state**).
  void (*init)(void **state);

  // update receives its state pointer (persistent across reloads) and time since last update (in seconds)
  void (*update)(void *state, float deltaTime);

  // shutdown is responsible for cleanly shutting down and freeing its own state memory (state**).
  void (*shutdown)(void **state);
} PluginAPI;

PluginAPI *get_plugin_api(void);

#ifdef __cplusplus
}
#endif

#endif
