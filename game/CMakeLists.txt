project(game LANGUAGES C)

if(ENABLE_HOT_RELOAD)
	add_library(game SHARED
		src/game.c
		include/game.h
	)
else()
	add_library(game STATIC
		src/game.c
		include/game.h
	)
endif()

target_include_directories(game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(game
	PRIVATE shared
)