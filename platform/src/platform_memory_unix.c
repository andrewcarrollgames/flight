#if defined(__unix__) || defined(__APPLE__)

#include "platform.h"
#include "platform_memory.h"
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/vm_statistics.h>
#endif

void *Platform_MemoryReserve(size_t size) {
  // Reserve address space without physical backing
  void *ptr = mmap(NULL, size, PROT_NONE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (ptr == MAP_FAILED) {
    Platform_LogError("mmap(reserve) failed: %s", strerror(errno));
    return NULL;
  }

  return ptr;
}

bool Platform_MemoryCommit(void *ptr, size_t size) {
  // Change protection to allow read/write
  if (mprotect(ptr, size, PROT_READ | PROT_WRITE) != 0) {
    Platform_LogError("mprotect(commit) failed: %s", strerror(errno));
    return false;
  }

  return true;
}

void Platform_MemoryDecommit(void *ptr, size_t size) {
  // Remove physical backing but keep reservation
#ifdef __linux__
  // On Linux, use madvise to tell kernel we don't need this memory
  if (madvise(ptr, size, MADV_DONTNEED) != 0) {
    Platform_LogError("madvise(decommit) failed: %s", strerror(errno));
  }
#endif

  // Change protection back to PROT_NONE
  if (mprotect(ptr, size, PROT_NONE) != 0) {
    Platform_LogError("mprotect(decommit) failed: %s", strerror(errno));
  }
}

void Platform_MemoryRelease(void *ptr, size_t size) {
  if (munmap(ptr, size) != 0) {
    Platform_LogError("munmap failed: %s", strerror(errno));
  }
}

PlatformMemoryStats Platform_MemoryGetStats(void) {
  PlatformMemoryStats stats = {0};

  // Page size
  stats.page_size = (size_t)sysconf(_SC_PAGESIZE);

  // On Unix, allocation granularity is same as page size
  stats.allocation_granularity = stats.page_size;

  // Physical memory
#ifdef __linux__
  long pages = sysconf(_SC_PHYS_PAGES);
  long avail_pages = sysconf(_SC_AVPHYS_PAGES);
  if (pages > 0 && avail_pages > 0) {
    stats.total_physical = (size_t)pages * stats.page_size;
    stats.available_physical = (size_t)avail_pages * stats.page_size;
  }
#elif defined(__APPLE__)
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  vm_statistics_data_t vm_stat;
  mach_port_t host_port = mach_host_self();

  if (host_statistics(host_port, HOST_VM_INFO,
                      (host_info_t)&vm_stat, &count) == KERN_SUCCESS) {
    stats.total_physical = (size_t)(vm_stat.wire_count +
                                    vm_stat.active_count +
                                    vm_stat.inactive_count +
                                    vm_stat.free_count) *
                           stats.page_size;
    stats.available_physical = (size_t)vm_stat.free_count * stats.page_size;
  }
#endif

  return stats;
}

#endif