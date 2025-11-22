project(shared LANGUAGES C)

# Shared is just interface headers
add_library(shared INTERFACE)

target_include_directories(shared INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Shared depends on generated headers existing
add_dependencies(shared generate_plugin_macros)