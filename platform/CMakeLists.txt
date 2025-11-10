project(platform LANGUAGES C)

# Platform selection (could come from presets or command line)
if(NOT PLATFORM_BACKEND)
	set(PLATFORM_BACKEND "SDL" CACHE STRING "Platform backend (SDL)")
endif()

# Headers for the platform API
set(PLATFORM_HEADERS
	include/platform.h
	include/platform_renderer.h
	include/platform_window.h
)

# =======================
# PART 1: Platform Library (the API implementation)
# =======================
set(PLATFORM_LIB_SOURCES "")

if(PLATFORM_BACKEND STREQUAL "SDL")
	list(APPEND PLATFORM_LIB_SOURCES
		src/sdl/platform_renderer_sdl.c
		src/sdl/platform_sdl.c
		src/sdl/platform_sdl_internal.h
		src/sdl/platform_window_sdl.c
	)

	set(BUILD_SHARED_LIBS OFF)
	set(SDL_SHARED OFF CACHE INTERNAL "")
	#set(SDL_STATIC ON CACHE INTERNAL "Enable PIC for static libraries" FORCE)
	add_subdirectory(extern/SDL)

	set(PLATFORM_LIBS SDL3::SDL3)
endif()

# Create the platform library (Engine will link this)
add_library(platform_lib STATIC ${PLATFORM_LIB_SOURCES} ${PLATFORM_HEADERS})

target_include_directories(platform_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(platform_lib PUBLIC ${PLATFORM_LIBS})

target_compile_definitions(platform_lib PUBLIC
	$<$<CONFIG:Debug>:FLIGHT_ENABLE_LOGGING>
	$<$<CONFIG:Release>:FLIGHT_ENABLE_LOGGING>
)

# =======================
# PART 2: Platform Executable (just the entry point)
# =======================
set(PLATFORM_EXE_SOURCES "")

if(PLATFORM_BACKEND STREQUAL "SDL")
	list(APPEND PLATFORM_EXE_SOURCES src/sdl/main_sdl.c)
endif()

# Create the executable
add_executable(platform WIN32 ${PLATFORM_EXE_SOURCES})

if(PLATFORM_BACKEND STREQUAL "SDL")
	target_compile_definitions(platform PRIVATE SDL_MAIN_USE_CALLBACKS)
endif()

# Executable name from game
set_target_properties(platform PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

# Add Emscripten-specific settings
if(EMSCRIPTEN)
	# Generate HTML file
	set_target_properties(platform PROPERTIES SUFFIX ".html")

	# Optional: Customize the shell
	# target_link_options(platform PRIVATE
	#     "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/path/to/custom_shell.html"
	# )
endif()

# Link the executable to both the platform library AND engine
target_link_libraries(platform 
	PRIVATE platform_lib   # Platform implementation
	PRIVATE engine         # Engine code
)
