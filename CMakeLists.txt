cmake_minimum_required(VERSION 3.5...4.0)

project(flight LANGUAGES C CXX)
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Persistent/shared data between the engine and game (supports hot reload)
# add_subdirectory(shared)

# Simulation code for the game (the piece that can be hot reloaded)
# add_subdirectory(game EXCLUDE_FROM_ALL)

# platform is our entry point (will instantiate and update engine)
# This should be provided by the config/preset, maybe.
set(PLATFORM_BACKEND "SDL")
add_subdirectory(platform)

# Check/assign platform variables here (or pass from config, maybe?)
add_subdirectory(engine)
