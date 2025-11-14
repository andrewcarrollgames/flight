#include "engine.h"
#include "platform.h"
#include <stdlib.h>

#ifdef ENABLE_HOT_RELOAD
    // Hot reload build - use plugin system
    #include "plugin_manager.h"
#else
    // Static build - direct link to game
    #include "game.h"
    static void* gameState = NULL;
#endif

bool Engine_Initialize(void) {
  Platform_Log("Engine Initializing.");

#ifdef ENABLE_HOT_RELOAD
  // Hot reload path - use plugin manager
  if (!PluginManager_Init()) {
    Platform_LogError("Failed to initialize plugin manager");
    return false;
  }

  // Load game plugin dynamically
#ifdef _WIN32
  const char* game_plugin = "game.dll";
#elif defined(__APPLE__)
  const char* game_plugin = "libgame.dylib";
#else
  const char* game_plugin = "libgame.so";
#endif

  int game_index = PluginManager_Load(game_plugin);
  if (game_index < 0) {
    Platform_LogError("Failed to load game plugin");
    return false;
  }
#else
  if (!Game_Initialize(&gameState)) {
    Platform_LogError("Failed to initialize game");
    return false;
  }
#endif

  return true;
}

void Engine_Update(float deltaTime) {
#ifdef ENABLE_HOT_RELOAD
  // Check for hot reloads
  PluginManager_CheckReloadAll();

  // Update all plugins
  PluginManager_UpdateAll(deltaTime);
#else
  // Direct call to statically linked game
  Game_Update(gameState, deltaTime);
#endif
}

void Engine_Render(void) {
#ifdef ENABLE_HOT_RELOAD
  // Render all plugins
  PluginManager_RenderAll();
#else
  // Direct call to statically linked game
  Game_Render(gameState);
#endif
}

void Engine_Shutdown(void) {
  Platform_Log("Engine Shutting Down.");

#ifdef ENABLE_HOT_RELOAD
  // Shutdown plugin system
  PluginManager_Shutdown();
#else
  // Shutdown statically linked game
  if (gameState) {
    Game_Shutdown(&gameState);
  }
#endif
}