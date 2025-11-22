cmake_minimum_required(VERSION 3.5...4.0)

project(flight LANGUAGES C CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Enable PIC globally for all static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Catch the case where Hot reload was enabled, but we forgot to enable game as a plugin
if(${ENABLE_HOT_RELOAD})
    message("flight/CMakeLists: Hot reload enabled for this configuration. Turn on 'ENABLE_GAME_AS_PLUGIN'.")
    set(ENABLE_GAME_AS_PLUGIN ON)
endif()

# ============================================================================
# STEP 1: Build code generation tools FIRST
# ============================================================================
add_subdirectory(tools)

# ============================================================================
# STEP 2: Generate extension APIs and macros
# ============================================================================
add_subdirectory(extensions)

# ============================================================================
# STEP 3: Build core (depends on generated headers)
# ============================================================================
add_subdirectory(shared)
add_subdirectory(platform)
add_subdirectory(engine)
add_subdirectory(game)