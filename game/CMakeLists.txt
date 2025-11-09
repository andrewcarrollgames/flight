project(game LANGUAGES C)

set(GAME_BUILD_TYPE STATIC)

if(${ENABLE_HOT_RELOAD})
    set(GAME_BUILD_TYPE SHARED)
endif()

add_library(game ${GAME_BUILD_TYPE}
	src/game.c include/game.h
)

# TODO: Differentiate between linked and hot-reload versions of "game" here.
target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(game
	PRIVATE engine
	PRIVATE shared
)
