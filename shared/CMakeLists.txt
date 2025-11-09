project(shared LANGUAGES C)

add_library(shared INTERFACE)

target_include_directories(shared INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # For when building the library itself
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/shared> # For when the library is installed
)

target_link_libraries(shared INTERFACE engine)
