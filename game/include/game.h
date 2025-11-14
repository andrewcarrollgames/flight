#ifndef GAME_H
#define GAME_H

#include "plugin_api.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declarations
typedef struct PluginAPI PluginAPI;

// PluginAPI Implementation (plugin_api.h)
bool Game_Initialize(void **state, PlatformAPI *platformAPI, EngineAPI *engineAPI);
void Game_Update(void *state, float deltaTime);
void Game_Render(void *state);
void Game_Shutdown(void **state);
// End PluginAPI

#ifdef __cplusplus
}
#endif

#endif
