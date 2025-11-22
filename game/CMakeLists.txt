project(game LANGUAGES C)

set(GAME_FILES
    src/game.c 
    include/game.h
    include/game_context.h
    include/game_state.h
)

if(${ENABLE_GAME_AS_PLUGIN})
    add_library(game SHARED ${GAME_FILES})
    target_compile_definitions(game PRIVATE ENABLE_GAME_AS_PLUGIN)
	if(${ENABLE_HOT_RELOAD})
		target_compile_definitions(game PRIVATE ENABLE_HOT_RELOAD)		
	endif()
else()
    add_library(game STATIC ${GAME_FILES})
endif()

target_include_directories(game PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game PRIVATE shared)

# Game depends on generated macros
add_dependencies(game generate_plugin_macros)