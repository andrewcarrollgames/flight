project(test_extension LANGUAGES C)

# Build test extension library
add_library(test_extension STATIC
    test_extension.c
)

# Add the source file to the global list
set_property(GLOBAL APPEND PROPERTY FLIGHT_EXTENSION_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/test_extension.c"
)

target_include_directories(test_extension PUBLIC
    ${CMAKE_SOURCE_DIR}/shared/include
)

target_link_libraries(test_extension PRIVATE shared)

# Depend on generated API header (created by parent CMakeLists.txt)
add_custom_target(test_extension_api 
    DEPENDS ${CMAKE_SOURCE_DIR}/shared/include/test_extension_api.h
)
add_dependencies(test_extension test_extension_api)