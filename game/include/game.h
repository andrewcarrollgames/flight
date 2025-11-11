#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// PluginAPI Implementation (plugin_api.h)
bool Game_Initialize(void **state);
void Game_Update(void *state, float delta_time);
void Game_Shutdown(void **state);
// End PluginAPI

#ifdef __cplusplus
}
#endif

#endif
