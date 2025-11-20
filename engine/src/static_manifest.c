#include "extension.h"
#include <stddef.h> // for NULL

extern ExtensionInterface g_extension_test;

void Engine_RegisterExtension(ExtensionInterface* ext);

void Engine_LoadStaticExtensions(void) {
  // TEMPORARY: All extensions to be included get added here.
  Engine_RegisterExtension(&g_extension_test);
}