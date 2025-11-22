#include "test_extension_api.h"
#include "extension.h"
#include "platform_api.h"

static PlatformAPI* g_platform = NULL;

EXTENSION_API void Test_LogHello(void) {
  if (g_platform) {
    g_platform->Log("Hello!");
  }
}

EXTENSION_API void Test_LogWorld(void) {
  if (g_platform) {
    g_platform->Log("XOXO\n--Test_Extension");
  }
}

static TestAPI g_test_api = {
  .LogHello = Test_LogHello,
  .LogWorld = Test_LogWorld
};

// --- The Extension Interface ---
bool Test_Init(EngineAPI* engine, PlatformAPI* platform) {
  g_platform = platform;
  platform->Log("Test Extension Initialized.");
  return true;
}

void Test_Update(float dt) {
}

void Test_Shutdown(void) {
  g_platform->Log("Test Extension Shutdown.");
}

void* Test_GetSpecificAPI(void) {
  return &g_test_api;
}

// Exported Symbol
ExtensionInterface g_extension_test = {
  .name = "Test",
  .Init = Test_Init,
  .Update = Test_Update,
  .Shutdown = Test_Shutdown,
  .GetSpecificAPI = Test_GetSpecificAPI
};