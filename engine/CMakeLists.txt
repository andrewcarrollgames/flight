project(engine LANGUAGES C)

get_property(EXTENSION_SOURCES GLOBAL PROPERTY FLIGHT_EXTENSION_SOURCES)

add_library(engine STATIC
    src/engine.c
    src/plugin_manager.c
    src/static_manifest.c
    ${EXTENSION_SOURCES}
    include/plugin_manager.h
)

# Start with base libraries
set(ENGINE_LIBS
    PRIVATE platform_lib
    PRIVATE shared
)

# Add all extension libraries
list(APPEND ENGINE_LIBS PRIVATE ${FLIGHT_EXTENSION_LIBS})

# Add game if statically linked
if(NOT ${ENABLE_GAME_AS_PLUGIN})
    list(APPEND ENGINE_LIBS PRIVATE game)
else()
    target_compile_definitions(engine ENABLE_GAME_AS_PLUGIN)
	if(${ENABLE_HOT_RELOAD})
		target_compile_definitions(engine ENABLE_HOT_RELOAD)
	endif()
endif()

# Export symbols for hot-reload builds
if(${ENABLE_GAME_AS_PLUGIN})
    set_target_properties(engine PROPERTIES
        ENABLE_EXPORTS ON
    )
endif()

target_include_directories(engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(engine ${ENGINE_LIBS})

# Engine depends on generated macros
add_dependencies(engine generate_plugin_macros)