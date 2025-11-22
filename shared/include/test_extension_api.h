// Auto-generated API header for Test extension
// Do not edit manually - regenerated from source files
//
// Generated from 2 functions:
//   - Test_LogHello
//   - Test_LogWorld

#ifndef TEST_EXTENSION_API_H
#define TEST_EXTENSION_API_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct EngineAPI EngineAPI;
typedef struct PlatformAPI PlatformAPI;

// Export macro for extension functions
#ifdef _WIN32
  #define EXTENSION_API __declspec(dllexport)
#else
  #define EXTENSION_API __attribute__((visibility("default")))
#endif

// Test extension API
typedef struct TestAPI {
    void (*LogHello)(void);
    void (*LogWorld)(void);
} TestAPI;

// Forward declarations for static builds
EXTENSION_API void Test_LogHello(void);
EXTENSION_API void Test_LogWorld(void);

#ifdef __cplusplus
}
#endif

#endif // TEST_EXTENSION_API_H
