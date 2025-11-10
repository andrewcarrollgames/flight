project(engine LANGUAGES C)

add_library(engine STATIC
    src/engine.c include/engine.h
)

set(ENGINE_LIBS
	PUBLIC platform_lib
	PRIVATE shared
)

if(NOT ${ENABLE_HOT_RELOAD})
	list(APPEND ENGINE_LIBS
		PRIVATE game
	)
else()
	message(STATUS "Adding cr to support hot reload.")
	add_subdirectory(extern/cr/cr EXCLUDE_FROM_ALL)

	# Add CR directory
	message(STATUS "crux/src: Adding cr includes.")
	target_include_directories(${PROJECT_NAME} PRIVATE extern/cr/cr)

	message(STATUS "crux/src: Linking cr.")
	list(APPEND ENGINE_LIBS
		PRIVATE cr
	)

	target_compile_definitions(engine PRIVATE ENABLE_HOT_RELOAD)
endif()

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(engine ${ENGINE_LIBS})
