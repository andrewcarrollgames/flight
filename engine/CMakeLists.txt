# Build the macro generator tool (using tools/macro_gen.c)
add_executable(macro_gen ${CMAKE_SOURCE_DIR}/tools/macro_gen.c)

# Set this to be built first
set_target_properties(macro_gen PROPERTIES
	EXCLUDE_FROM_ALL FALSE
)

# Set output path for generated macros
set(SHARED_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/shared/include")
set(GENERATED_MACROS "${SHARED_INCLUDE_DIR}/plugin_macros_generated.h")

# Find all extension API headers
file(GLOB EXTENSION_API_HEADERS "${SHARED_INCLUDE_DIR}/*_extension_api.h")

# Generate macros before building engine
add_custom_command(
	OUTPUT ${GENERATED_MACROS}
	COMMAND $<TARGET_FILE:macro_gen> ${SHARED_INCLUDE_DIR} ${GENERATED_MACROS}
	DEPENDS macro_gen ${EXTENSION_API_HEADERS}
	COMMENT "Generating plugin macros from extension APIs"
	VERBATIM
)

# Create a target that always runs and depends on macro_gen being built
add_custom_target(generate_plugin_macros
	DEPENDS ${GENERATED_MACROS} macro_gen
)

# Now do the engine setup.
add_library(engine STATIC
	src/engine.c
	src/plugin_manager.c
	src/static_manifest.c
	../extensions/test/test_extension.c
	include/plugin_manager.h
)

# Make engine wait for the generation to complete
add_dependencies(engine generate_plugin_macros)

set(ENGINE_LIBS
	PRIVATE platform_lib
	PRIVATE shared
)

if(${ENABLE_HOT_RELOAD})
	message("engine/CMakeLists.txt: Hot Reload is enabled.")
	target_compile_definitions(engine PRIVATE ENABLE_HOT_RELOAD)
endif()

if(${ENABLE_GAME_AS_PLUGIN})
	message("engine/CMakeLists.txt: Game enabled as plugin.")
	target_compile_definitions(engine PRIVATE ENABLE_GAME_AS_PLUGIN)
else ()
	message("engine/CMakeLists.txt: Game will be linked statically.")
	list(APPEND ENGINE_LIBS
		PRIVATE game
	)
endif()

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(engine ${ENGINE_LIBS})
