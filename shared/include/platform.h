// Copyright (c) 2025 Andrew Carroll Games, LLC
// All rights reserved.

#ifndef FLIGHT_PLATFORM_H
#define FLIGHT_PLATFORM_H

#include <stdint.h>

/*
Memory: platform_alloc, platform_free, platform_realloc (wrapper around your allocator strategy)
Logging: platform_log, platform_log_error, platform_log_warn (with printf-style formatting)
Timing: platform_get_time, platform_get_delta_time, platform_sleep
File I/O: platform_read_file, platform_write_file, platform_file_exists, platform_get_file_size
Threading: platform_create_thread, platform_mutex_create/lock/unlock, platform_semaphore_*

Nice to Have:

Window/Display info: platform_get_window_size, platform_set_window_title
Input state queries: Maybe just expose SDL's input directly, or wrap it minimally
Dynamic library loading: platform_load_library, platform_unload_library, platform_get_proc_address (for hot reload!)
Path utilities: platform_get_base_path, platform_get_pref_path
 */

#ifdef __cplusplus
extern "C" {
#endif

void Platform_Log(const char *fmt, ...);
void Platform_LogError(const char *fmt, ...);
void Platform_LogWarning(const char *fmt, ...);
const char *Platform_GetBasePath();
const char *Platform_GetPrefPath(const char *org, const char *app);
uint64_t Platform_GetTicksNS();

#ifdef __cplusplus
}
#endif

#endif
