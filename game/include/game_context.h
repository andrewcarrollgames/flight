#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include "plugin_api.h"
#include "plugin_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

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
