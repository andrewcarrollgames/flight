#ifdef _WIN32

#include "platform.h"
#include "platform_memory.h"
#include <windows.h>

void *Platform_MemoryReserve(size_t size) {
  void *ptr = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_NOACCESS);

  if (!ptr) {
    Platform_LogError("VirtualAlloc(MEM_RESERVE) failed: %lu", GetLastError());
  }

  return ptr;
}

bool Platform_MemoryCommit(void *ptr, size_t size) {
  void *result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);

  if (!result) {
    Platform_LogError("VirtualAlloc(MEM_COMMIT) failed: %lu", GetLastError());
    return false;
  }

  return true;
}

void Platform_MemoryDecommit(void *ptr, size_t size) {
  if (!VirtualFree(ptr, size, MEM_DECOMMIT)) {
    Platform_LogError("VirtualFree(MEM_DECOMMIT) failed: %lu", GetLastError());
  }
}

void Platform_MemoryRelease(void *ptr, size_t size) {
  (void)size; // Not needed for MEM_RELEASE

  if (!VirtualFree(ptr, 0, MEM_RELEASE)) {
    Platform_LogError("VirtualFree(MEM_RELEASE) failed: %lu", GetLastError());
  }
}

PlatformMemoryStats Platform_MemoryGetStats(void) {
  PlatformMemoryStats stats = {0};

  SYSTEM_INFO si;
  GetSystemInfo(&si);

  stats.page_size = si.dwPageSize;
  stats.allocation_granularity = si.dwAllocationGranularity;

  MEMORYSTATUSEX mem_status;
  mem_status.dwLength = sizeof(mem_status);
  if (GlobalMemoryStatusEx(&mem_status)) {
    stats.total_physical = (size_t)mem_status.ullTotalPhys;
    stats.available_physical = (size_t)mem_status.ullAvailPhys;
  }

  return stats;
}

#endif