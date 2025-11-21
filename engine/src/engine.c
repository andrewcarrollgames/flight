// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "engine.h"
#include "engine_api.h"
#include "extension.h"
#include "platform.h"
#include "platform_api.h"
#include <stdlib.h>
#include <string.h>

// TODO: (ARC) Make this a dynamic array or size to the exact number of extensions we have (should be known at compile time)?
#define MAX_EXTENSIONS 32
static ExtensionInterface* g_extensions[MAX_EXTENSIONS];
static int g_extension_count = 0;

static void* Engine_GetExtensionAPI(const char* name) {
  for (int i = 0; i < g_extension_count; ++i) {
    if (strcmp(g_extensions[i]->name, name) == 0) {
      return g_extensions[i]->GetSpecificAPI();
    }
  }
  return NULL;
}

// Update global API instance
static EngineAPI g_engine_api = {
  .GetExtensionAPI = Engine_GetExtensionAPI
};

// Global function (called by Manifest)
void Engine_RegisterExtension(ExtensionInterface* ext) {
  if (g_extension_count < MAX_EXTENSIONS) {
    g_extensions[g_extension_count++] = ext;
    // Init immediately
    if (ext->Init) {
      ext->Init(&g_engine_api, Platform_GetAPI());
    }
  }
}

// TODO: (ARC) These add a significant amount of validation(branching) & dispatch as we add extensions.
// Probably better to just register valid update calls into a concrete "extension update" list in Engine_RegisterExtension.
void Engine_UpdateStaticExtensions(const float dt) {
  int extensionIdx = 0;
  for (extensionIdx = 0; extensionIdx < g_extension_count; ++extensionIdx) {
    const ExtensionInterface *ext = g_extensions[extensionIdx];
    if (ext->Update) {
      ext->Update(dt);
    }
  }
}

void Engine_ShutdownStaticExtensions() {
  int extensionIdx = 0;
  for (extensionIdx = 0; extensionIdx < g_extension_count; ++extensionIdx) {
    const ExtensionInterface *ext = g_extensions[extensionIdx];
    if (ext->Shutdown) {
      ext->Shutdown();
    }
  }
}

EngineAPI* Engine_GetAPI(void) {
  return &g_engine_api;
}

#ifdef ENABLE_GAME_AS_PLUGIN
    // Hot reload build - use plugin system
    #include "plugin_manager.h"
#else
    // Static build - direct link to game
    #include "game.h"
    static void* gameState = NULL;
#endif

bool Engine_Initialize(void) {
  Platform_Log("Engine Initializing.");

  // Forward declaration (function defined in static_manifest.c)
  void Engine_LoadStaticExtensions(void);
  Engine_LoadStaticExtensions();

#ifdef ENABLE_GAME_AS_PLUGIN
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

  const char* basePath = Platform_GetBasePath();
  size_t base_len = strlen(basePath);
  size_t plugin_len = strlen(game_plugin);
  size_t total_len = base_len + plugin_len + 1;

  // Stack allocate - no arena needed for such a small temp
  char fullPath[512]; // Reasonable max path length
  if (total_len > sizeof(fullPath)) {
    Platform_LogError("Plugin path too long");
    return false;
  }

  strcpy(fullPath, basePath);
  strcat(fullPath, game_plugin);

  Platform_Log("Loading plugin @: %s", fullPath);

  int game_index = PluginManager_Load(fullPath);
  if (game_index < 0) {
    Platform_LogError("Failed to load game plugin");
    return false;
  }

#else
  if (!Game_Initialize(&gameState, NULL, NULL)) {
    Platform_LogError("Failed to initialize game");
    return false;
  }
#endif

  return true;
}

void Engine_Update(float deltaTime) {
#ifdef ENABLE_GAME_AS_PLUGIN

  // Only include hot reload checks if ordered to at compile time by the config.
  #ifdef ENABLE_HOT_RELOAD
    // Check for hot reloads
    PluginManager_CheckReloadAll();
  #endif

  // Update all plugins
  PluginManager_UpdateAll(deltaTime);
#else
  // Direct call to statically linked game
  Game_Update(gameState, deltaTime);
#endif

  Engine_UpdateStaticExtensions(deltaTime);
}

void Engine_Render(void) {
#ifdef ENABLE_GAME_AS_PLUGIN
  // Render all plugins
  PluginManager_RenderAll();
#else
  // Direct call to statically linked game
  Game_Render(gameState);
#endif
}

void Engine_Shutdown(void) {
  Platform_Log("Engine Shutting Down.");

#ifdef ENABLE_GAME_AS_PLUGIN
  // Shutdown plugin system
  PluginManager_Shutdown();
#else
  // Shutdown statically linked game
  if (gameState) {
    Game_Shutdown(&gameState);
  }
#endif

  Engine_ShutdownStaticExtensions();
}