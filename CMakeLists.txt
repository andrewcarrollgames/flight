cmake_minimum_required(VERSION 3.5...4.0)

project(flight LANGUAGES C CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# platform is our entry point (will instantiate and update engine)
add_subdirectory(platform)

# Persistent/shared data between the engine and game (supports hot reload)
add_subdirectory(shared)

# Check/assign platform variables here (or pass from config, maybe?)
add_subdirectory(engine)

# Simulation code for the game (the piece that can be hot reloaded)
if(NOT ${ENABLE_HOT_RELOAD})
    add_subdirectory(game)
endif()
