project(engine LANGUAGES C)

add_library(engine STATIC
    src/engine.c
	src/plugin_manager.c
	src/static_manifest.c
	../extensions/test/test_extension.c #<-Temporary (for verification)
	include/plugin_manager.h
)

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
