#ifndef TEST_EXTENSIONS_H
#define TEST_EXTENSIONS_H

#include <stdbool.h>

// --- The Specific API ---
typedef struct TestAPI {
  void (*LogHello)(void);
  void (*LogWorld)(void);
} TestAPI;

// forward declarations
typedef struct EngineAPI EngineAPI;
typedef struct PlatformAPI PlatformAPI;

#ifdef __cplusplus
extern "C" {
#endif
  void Test_LogHello(void);
  void Test_LogWorld(void);
  void* Test_GetSpecificAPI(void);
#ifdef __cplusplus
}
#endif

#endif
