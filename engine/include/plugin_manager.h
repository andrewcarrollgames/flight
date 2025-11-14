#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

  // Initialize plugin system
  bool PluginManager_Init(void);

  // Shutdown plugin system and unload all plugins
  void PluginManager_Shutdown(void);

  // Load a plugin from path (returns plugin index, or -1 on failure)
  int PluginManager_Load(const char* path);

  // Unload a specific plugin
  void PluginManager_Unload(int plugin_index);

  // Update all loaded plugins
  void PluginManager_UpdateAll(float delta_time);

  // Render all loaded plugins
  void PluginManager_RenderAll(void);

  // Check and reload any plugins that changed
  void PluginManager_CheckReloadAll(void);

  // Get number of loaded plugins
  int PluginManager_GetCount(void);

#ifdef __cplusplus
}
#endif

#endif