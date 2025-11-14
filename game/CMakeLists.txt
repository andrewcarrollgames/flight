project(game LANGUAGES C)

set(GAME_FILES
	src/game.c include/game.h
	include/game_context.h
	include/game_state.h
)

if(ENABLE_HOT_RELOAD)
	add_library(game SHARED ${GAME_FILES})
	target_compile_definitions(game PRIVATE ENABLE_HOT_RELOAD)
else()
	add_library(game STATIC ${GAME_FILES})
endif()

target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(game
	PRIVATE shared
)