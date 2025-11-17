// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#include "platform_plugin.h"
#include "platform.h"
#include <dlfcn.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

struct PlatformPlugin {
    void* handle;
    char path[256];
    char temp_path[256];
    time_t last_mtime;
};

PlatformPlugin* Platform_PluginLoad(const char* path) {
    PlatformPlugin* plugin = (PlatformPlugin*)malloc(sizeof(PlatformPlugin));
    if (!plugin) {
        Platform_LogError("Failed to allocate plugin structure");
        return NULL;
    }

    strncpy(plugin->path, path, sizeof(plugin->path) - 1);
    plugin->path[sizeof(plugin->path) - 1] = '\0';

    // Create temp copy path
    snprintf(plugin->temp_path, sizeof(plugin->temp_path),
             "%s.%ld.tmp", path, (long)time(NULL));

    // Copy plugin to temp location
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "cp '%s' '%s'", path, plugin->temp_path);
    if (system(cmd) != 0) {
        Platform_LogError("Failed to copy plugin: %s", path);
        free(plugin);
        return NULL;
    }

    // Load the temp copy
    plugin->handle = dlopen(plugin->temp_path, RTLD_NOW | RTLD_LOCAL);
    if (!plugin->handle) {
        Platform_LogError("Failed to load plugin: %s - %s", plugin->temp_path, dlerror());
        unlink(plugin->temp_path);
        free(plugin);
        return NULL;
    }

    // Store file time for reload checking
    struct stat st;
    if (stat(path, &st) == 0) {
        plugin->last_mtime = st.st_mtime;
    }

    Platform_Log("Loaded plugin from: %s", path);
    return plugin;
}

void Platform_PluginUnload(PlatformPlugin* plugin) {
    if (!plugin) return;

    if (plugin->handle) {
        dlclose(plugin->handle);
        plugin->handle = NULL;
    }

    // Delete temp file
    unlink(plugin->temp_path);

    free(plugin);
}

void* Platform_PluginGetSymbol(PlatformPlugin* plugin, const char* name) {
    if (!plugin || !plugin->handle) {
        Platform_LogError("Invalid plugin handle");
        return NULL;
    }

    // Clear any existing error
    dlerror();

    void* symbol = dlsym(plugin->handle, name);

    const char* error = dlerror();
    if (error) {
        Platform_LogError("Symbol not found: %s - %s", name, error);
        return NULL;
    }

    return symbol;
}

bool Platform_PluginNeedsReload(PlatformPlugin* plugin) {
    if (!plugin) return false;

    struct stat st;
    if (stat(plugin->path, &st) != 0) {
        return false;
    }

    // Check if file was modified
    return st.st_mtime > plugin->last_mtime;
}

bool Platform_PluginReload(PlatformPlugin* plugin) {
  if (!plugin) return false;

  Platform_Log("Reloading plugin: %s", plugin->path);

  // Save the old temp path
  char old_temp_path[256];
  strncpy(old_temp_path, plugin->temp_path, sizeof(old_temp_path) - 1);
  old_temp_path[sizeof(old_temp_path) - 1] = '\0';

  // Unload old handle
  if (plugin->handle) {
    dlclose(plugin->handle);
    plugin->handle = NULL;
  }

  // Sleep to let OS release file handles
  usleep(50000);  // 50ms in microseconds

  // Create new temp path
  snprintf(plugin->temp_path, sizeof(plugin->temp_path),
           "%s.%ld.tmp", plugin->path, (long)time(NULL));

  // Copy new version to temp
  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "cp '%s' '%s'", plugin->path, plugin->temp_path);
  if (system(cmd) != 0) {
    Platform_LogError("Failed to copy plugin during reload: %s", plugin->path);

    // Try to reload the old temp file as fallback
    Platform_Log("Attempting to reload old version...");
    plugin->handle = dlopen(old_temp_path, RTLD_NOW | RTLD_LOCAL);
    if (plugin->handle) {
      strncpy(plugin->temp_path, old_temp_path, sizeof(plugin->temp_path) - 1);
      Platform_LogWarning("Reloaded old version successfully");
      return false;
    }

    return false;
  }

  // Delete old temp file now that we have a new one
  unlink(old_temp_path);

  // Load new version
  plugin->handle = dlopen(plugin->temp_path, RTLD_NOW | RTLD_LOCAL);
  if (!plugin->handle) {
    Platform_LogError("Failed to load plugin during reload: %s - %s",
                     plugin->temp_path, dlerror());
    return false;
  }

  // Update file time
  struct stat st;
  if (stat(plugin->path, &st) == 0) {
    plugin->last_mtime = st.st_mtime;
  }

  Platform_Log("Plugin reloaded successfully: %s", plugin->temp_path);
  return true;
}