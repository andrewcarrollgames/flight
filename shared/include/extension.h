#ifndef EXTENSION_H
#define EXTENSION_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Extension function export macro
#ifdef _WIN32
	#define EXTENSION_API __declspec(dllexport)
#else
	#define EXTENSION_API __attribute__((visibility("default")))
#endif

// Forward declarations
typedef struct EngineAPI EngineAPI;
typedef struct PlatformAPI PlatformAPI;

// The interface for compile-time engine extensions. These are bolted on to the core engine statically in
// the specified configuration and are used to add core functionality for both core and plugins.
typedef struct ExtensionInterface {
  const char *name;
  uint32_t version;

  // Lifecycle hooks
  bool (*Init)(EngineAPI *engine, PlatformAPI *platform);
  void (*Update)(float dt);
  void (*Shutdown)(void);

  // The Payload: Returns the specific API struct
  void *(*GetSpecificAPI)(void);
} ExtensionInterface;

#ifdef __cplusplus
}
#endif

#endif
