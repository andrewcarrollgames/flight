#ifndef GAME_H
#define GAME_H

#include "plugin_api.h"
#include "plugin_macros.h"
//#include "platform_api.h"
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

static PluginAPI g_game_plugin = {
    .version = 1,
    .name = "Flight Game",
    .init = Game_Initialize,
    .update = Game_Update,
    .render = Game_Render,
    .shutdown = Game_Shutdown
};

DEFINE_PLUGIN_API_ACCESSORS(g_game_plugin);

#ifdef __cplusplus
}
#endif

#endif
