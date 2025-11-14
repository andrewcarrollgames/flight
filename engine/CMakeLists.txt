project(engine LANGUAGES C)

add_library(engine STATIC
    src/engine.c
	src/plugin_manager.c
	include/plugin_manager.h
)

set(ENGINE_LIBS
	PRIVATE platform_lib
	PRIVATE shared
)

if(NOT ${ENABLE_HOT_RELOAD})
	list(APPEND ENGINE_LIBS
		PRIVATE game
	)
else()
	target_compile_definitions(engine PRIVATE ENABLE_HOT_RELOAD)
endif()

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(engine ${ENGINE_LIBS})
