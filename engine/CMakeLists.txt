project(engine LANGUAGES C)

add_library(${PROJECT_NAME} STATIC
        src/engine.c include/engine.h
)

# TODO: Differentiate between linked and hot-reload versions of "game" here.
# TODO: Add shared library.
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(engine
        PUBLIC platform_lib
)
