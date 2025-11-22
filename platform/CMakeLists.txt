project(platform LANGUAGES C)

# Platform selection (could come from presets or command line)
if(NOT PLATFORM_BACKEND)
    set(PLATFORM_BACKEND "SDL" CACHE STRING "Platform backend (SDL)")
endif()

# =======================
# PART 1: Platform Library (the API implementation)
# =======================
set(PLATFORM_LIB_SOURCES
    include/platform_memory.h
    src/arena.c
    src/vector2.c
)

if(UNIX AND NOT EMSCRIPTEN)
    list(APPEND PLATFORM_LIB_SOURCES
        src/platform_memory_unix.c
        src/platform_plugin_unix.c
    )
elseif(WIN32)
    list(APPEND PLATFORM_LIB_SOURCES
        src/platform_memory_win32.c
        src/platform_plugin_win32.c
    )
elseif(EMSCRIPTEN)
    list(APPEND PLATFORM_LIB_SOURCES
        src/platform_memory_wasm.c
    )
endif()

if(PLATFORM_BACKEND STREQUAL "SDL")
    list(APPEND PLATFORM_LIB_SOURCES
        src/sdl/platform_renderer_sdl.c
        src/sdl/platform_sdl.c
        src/sdl/platform_sdl_internal.h
        src/sdl/platform_window_sdl.c
    )

    set(BUILD_SHARED_LIBS OFF)
    set(SDL_SHARED OFF CACHE INTERNAL "")
    add_subdirectory(extern/SDL)

    set(PLATFORM_LIBS PUBLIC SDL3::SDL3 PRIVATE shared)
endif()

# Create the platform library
add_library(platform_lib STATIC ${PLATFORM_LIB_SOURCES})

# Export symbols for hot-reload builds
if(${ENABLE_GAME_AS_PLUGIN})
    set_target_properties(platform_lib PROPERTIES
        ENABLE_EXPORTS ON
    )
    if(UNIX)
        target_link_options(platform_lib PRIVATE "-rdynamic")
    endif()
endif()

target_include_directories(platform_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(platform_lib PUBLIC ${PLATFORM_LIBS})

target_compile_definitions(platform_lib PUBLIC
    $<$<CONFIG:Debug>:FLIGHT_ENABLE_LOGGING>
    $<$<CONFIG:RelWithDebInfo>:FLIGHT_ENABLE_LOGGING>
)

# Platform library depends on generated macros
add_dependencies(platform_lib generate_plugin_macros)

# =======================
# PART 2: Platform Executable (entry point)
# =======================
set(PLATFORM_EXE_SOURCES "")

if(PLATFORM_BACKEND STREQUAL "SDL")
    list(APPEND PLATFORM_EXE_SOURCES src/sdl/main_sdl.c)
endif()

# Create the executable
add_executable(platform WIN32 ${PLATFORM_EXE_SOURCES})

# Executable name from cache variable
set_target_properties(platform PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

# Add Emscripten-specific settings
if(EMSCRIPTEN)
    set_target_properties(platform PROPERTIES SUFFIX ".html")
endif()

# Link the executable
target_link_libraries(platform
    PRIVATE shared
    PRIVATE platform_lib
    PRIVATE engine
)