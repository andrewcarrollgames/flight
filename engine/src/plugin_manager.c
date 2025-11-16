#include "plugin_manager.h"
#include "plugin_api.h"
#include "platform_api.h"
#include "engine_api.h"
#include "platform.h"
#include "platform_plugin.h"
#include <string.h>

#define MAX_PLUGINS 32

typedef struct {
    PlatformPlugin* handle;
    PluginAPI* api;
    void* state;
    bool active;
    char path[256];
} LoadedPlugin;

static LoadedPlugin g_plugins[MAX_PLUGINS];
static int g_plugin_count = 0;
static bool g_initialized = false;

bool PluginManager_Init(void) {
    if (g_initialized) {
        Platform_LogWarning("Plugin manager already initialized");
        return true;
    }

    memset(g_plugins, 0, sizeof(g_plugins));
    g_plugin_count = 0;
    g_initialized = true;

    Platform_Log("Plugin manager initialized");
    return true;
}

void PluginManager_Shutdown(void) {
    if (!g_initialized) return;

    // Unload all plugins in reverse order
    for (int i = g_plugin_count - 1; i >= 0; i--) {
        if (g_plugins[i].active) {
            PluginManager_Unload(i);
        }
    }

    g_plugin_count = 0;
    g_initialized = false;

    Platform_Log("Plugin manager shutdown");
}

int PluginManager_Load(const char* path) {
    if (!g_initialized) {
        Platform_LogError("Plugin manager not initialized");
        return -1;
    }

    if (g_plugin_count >= MAX_PLUGINS) {
        Platform_LogError("Maximum plugins loaded (%d)", MAX_PLUGINS);
        return -1;
    }

    LoadedPlugin* plugin = &g_plugins[g_plugin_count];

    // Load the DLL
    plugin->handle = Platform_PluginLoad(path);
    if (!plugin->handle) {
        Platform_LogError("Failed to load plugin: %s", path);
        return -1;
    }

    // Get the API function
    typedef PluginAPI* (*GetAPIFunc)(void);
    const GetAPIFunc get_api = (GetAPIFunc)Platform_PluginGetSymbol(plugin->handle, "GetPluginAPI");
    if (!get_api) {
        Platform_LogError("Plugin missing GetPluginAPI: %s", path);
        Platform_PluginUnload(plugin->handle);
        return -1;
    }

    // Get the plugin API
    plugin->api = get_api();
    if (!plugin->api) {
        Platform_LogError("GetPluginAPI returned NULL: %s", path);
        Platform_PluginUnload(plugin->handle);
        return -1;
    }

    // Initialize the plugin
    if (plugin->api->init) {
        if (!plugin->api->init(&plugin->state, Platform_GetAPI(), Engine_GetAPI())) {
            Platform_LogError("Plugin init failed: %s", path);
            Platform_PluginUnload(plugin->handle);
            return -1;
        }
    }

    // Store path for reload checking
    strncpy(plugin->path, path, sizeof(plugin->path) - 1);
    plugin->active = true;

    int index = g_plugin_count++;

    Platform_Log("Loaded plugin: %s (v%d) - %s",
                 plugin->api->name,
                 plugin->api->version,
                 path);

    return index;
}

void PluginManager_Unload(int plugin_index) {
    if (plugin_index < 0 || plugin_index >= g_plugin_count) {
        Platform_LogError("Invalid plugin index: %d", plugin_index);
        return;
    }

    LoadedPlugin* plugin = &g_plugins[plugin_index];
    if (!plugin->active) return;

    Platform_Log("Unloading plugin: %s", plugin->api->name);

    // Shutdown the plugin
    if (plugin->api->shutdown && plugin->state) {
        plugin->api->shutdown(&plugin->state);
    }

    // Unload the DLL
    Platform_PluginUnload(plugin->handle);

    plugin->active = false;
    plugin->api = NULL;
    plugin->state = NULL;
    plugin->handle = NULL;
}

void PluginManager_UpdateAll(float delta_time) {
    for (int i = 0; i < g_plugin_count; i++) {
        const LoadedPlugin* plugin = &g_plugins[i];

        if (!plugin->active || !plugin->api->update) continue;

        plugin->api->update(plugin->state, delta_time);
    }
}

void PluginManager_RenderAll(void) {
    for (int i = 0; i < g_plugin_count; i++) {
        const LoadedPlugin* plugin = &g_plugins[i];

        if (!plugin->active || !plugin->api->render) continue;

        plugin->api->render(plugin->state);
    }
}

void PluginManager_CheckReloadAll(void) {
    for (int i = 0; i < g_plugin_count; i++) {
        LoadedPlugin* plugin = &g_plugins[i];

        if (!plugin->active) continue;

        // Check if plugin DLL changed
        if (Platform_PluginNeedsReload(plugin->handle)) {
            Platform_Log("Reloading plugin: %s", plugin->api->name);

            // Save state pointer (survives reload)
            void* saved_state = plugin->state;

            // Reload the DLL
            if (!Platform_PluginReload(plugin->handle)) {
                Platform_LogError("Failed to reload plugin: %s", plugin->path);
                continue;
            }

            // Get new API
            typedef PluginAPI* (*GetAPIFunc)(void);
            const GetAPIFunc get_api = (GetAPIFunc)Platform_PluginGetSymbol(plugin->handle, "GetPluginAPI");
            if (!get_api) {
                Platform_LogError("Plugin missing GetPluginAPI after reload");
                continue;
            }

            plugin->api = get_api();
            plugin->state = saved_state;  // Restore state pointer
            plugin->api->platform = Platform_GetAPI();
            plugin->api->engine = Engine_GetAPI();

            Platform_Log("Plugin reloaded: %s (v%d)",
                         plugin->api->name,
                         plugin->api->version);
        }
    }
}

int PluginManager_GetCount(void) {
    return g_plugin_count;
}
