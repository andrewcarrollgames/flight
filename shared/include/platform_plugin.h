#ifndef PLATFORM_PLUGIN_H
#define PLATFORM_PLUGIN_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlatformPlugin PlatformPlugin;

// Load a plugin DLL/SO
PlatformPlugin* Platform_PluginLoad(const char* path);

// Unload a plugin
void Platform_PluginUnload(PlatformPlugin* plugin);

// Get a symbol (function pointer) from plugin
void* Platform_PluginGetSymbol(PlatformPlugin* plugin, const char* name);

// Check if plugin file has changed (for hot reload)
bool Platform_PluginNeedsReload(PlatformPlugin* plugin);

// Reload a plugin (unload and load again)
bool Platform_PluginReload(PlatformPlugin* plugin);

#ifdef __cplusplus
}
#endif

#endif