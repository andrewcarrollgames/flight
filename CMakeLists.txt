cmake_minimum_required(VERSION 3.5...4.0)

project(flight LANGUAGES C CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Enable PIC globally for all static libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(${ENABLE_HOT_RELOAD})
    message("flight/CMakeLists: Hot reload enabled for this configuration. Turn on 'ENABLE_GAME_AS_PLUGIN'.")
    set(ENABLE_GAME_AS_PLUGIN ON)
endif()

# platform is our entry point (will instantiate and update engine)
add_subdirectory(platform)

# Persistent/shared data between the engine and game (supports hot reload)
add_subdirectory(shared)

# Check/assign platform variables here (or pass from config, maybe?)
add_subdirectory(engine)

# Simulation code for the game (the piece that can be hot reloaded)
add_subdirectory(game)
