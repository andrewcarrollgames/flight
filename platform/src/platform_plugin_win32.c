#include "platform_plugin.h"
#include "platform.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

struct PlatformPlugin {
    HMODULE dll;
    char path[MAX_PATH];
    char temp_path[MAX_PATH];
    FILETIME last_write_time;
};

PlatformPlugin* Platform_PluginLoad(const char* path) {
    PlatformPlugin* plugin = (PlatformPlugin*)malloc(sizeof(PlatformPlugin));
    if (!plugin) {
        Platform_LogError("Failed to allocate plugin structure");
        return NULL;
    }

    strncpy(plugin->path, path, MAX_PATH - 1);
    plugin->path[MAX_PATH - 1] = '\0';

    // Create temp copy path (so original can be rebuilt)
    snprintf(plugin->temp_path, MAX_PATH, "%s.%llu.tmp", path, (unsigned long long)time(NULL));

    // Copy DLL to temp location
    if (!CopyFileA(path, plugin->temp_path, FALSE)) {
        Platform_LogError("Failed to copy plugin: %s", path);
        free(plugin);
        return NULL;
    }

    // Load the temp copy
    plugin->dll = LoadLibraryA(plugin->temp_path);
    if (!plugin->dll) {
        Platform_LogError("Failed to load plugin: %s (error: %lu)", plugin->temp_path, GetLastError());
        DeleteFileA(plugin->temp_path);
        free(plugin);
        return NULL;
    }

    // Store file time for reload checking
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        GetFileTime(file, NULL, NULL, &plugin->last_write_time);
        CloseHandle(file);
    }

    Platform_Log("Loaded plugin from: %s", path);
    return plugin;
}

void Platform_PluginUnload(PlatformPlugin* plugin) {
    if (!plugin) return;

    if (plugin->dll) {
        FreeLibrary(plugin->dll);
        plugin->dll = NULL;
    }

    // Delete temp file
    DeleteFileA(plugin->temp_path);

    free(plugin);
}

void* Platform_PluginGetSymbol(PlatformPlugin* plugin, const char* name) {
    if (!plugin || !plugin->dll) {
        Platform_LogError("Invalid plugin handle");
        return NULL;
    }

    void* symbol = (void*)GetProcAddress(plugin->dll, name);
    if (!symbol) {
        Platform_LogError("Symbol not found: %s", name);
    }

    return symbol;
}

bool Platform_PluginNeedsReload(PlatformPlugin* plugin) {
    if (!plugin) return false;

    HANDLE file = CreateFileA(plugin->path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    FILETIME current_time;
    GetFileTime(file, NULL, NULL, &current_time);
    CloseHandle(file);

    // Check if file was modified
    return CompareFileTime(&current_time, &plugin->last_write_time) != 0;
}

bool Platform_PluginReload(PlatformPlugin* plugin) {
    if (!plugin) return false;

    Platform_Log("Reloading plugin: %s", plugin->path);

    // Unload old DLL
    if (plugin->dll) {
        FreeLibrary(plugin->dll);
        plugin->dll = NULL;
    }

    // Delete old temp file
    DeleteFileA(plugin->temp_path);

    // Create new temp path
    snprintf(plugin->temp_path, MAX_PATH, "%s.%llu.tmp",
             plugin->path, (unsigned long long)time(NULL));

    // Copy new version to temp
    if (!CopyFileA(plugin->path, plugin->temp_path, FALSE)) {
        Platform_LogError("Failed to copy plugin during reload: %s", plugin->path);
        return false;
    }

    // Load new version
    plugin->dll = LoadLibraryA(plugin->temp_path);
    if (!plugin->dll) {
        Platform_LogError("Failed to load plugin during reload: %s (error: %lu)",
                         plugin->temp_path, GetLastError());
        return false;
    }

    // Update file time
     HANDLE file = CreateFileA(plugin->path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        GetFileTime(file, NULL, NULL, &plugin->last_write_time);
        CloseHandle(file);
    }

    Platform_Log("Plugin reloaded successfully: %s", plugin->path);
    return true;
}