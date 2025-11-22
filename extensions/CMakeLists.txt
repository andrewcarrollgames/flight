# Process all extensions and generate their API headers
# This runs AFTER tools are built, BEFORE platform/engine

set(SHARED_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/shared/include")

# ============================================================================
# Scan for all extension directories
# ============================================================================
file(GLOB EXTENSION_DIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*")

set(ALL_EXTENSION_APIS "")
set(ALL_EXTENSION_LIBS "")

foreach(EXT_DIR ${EXTENSION_DIRS})
    if(IS_DIRECTORY ${EXT_DIR})
        get_filename_component(EXT_NAME ${EXT_DIR} NAME)
        
        # Skip common non-extension directories
        if(EXT_NAME STREQUAL "CMakeFiles" OR 
           EXT_NAME STREQUAL ".git" OR
           EXT_NAME STREQUAL "build")
            continue()
        endif()
        
        message(STATUS "Processing extension: ${EXT_NAME}")
        
        # Define output API header path
        set(EXT_API_HEADER "${SHARED_INCLUDE_DIR}/${EXT_NAME}_extension_api.h")
        
        # Find all .c files in this extension directory
        file(GLOB EXT_SOURCES "${EXT_DIR}/*.c")
        
        if(EXT_SOURCES)
            # Generate API header for this extension
            add_custom_command(
                OUTPUT ${EXT_API_HEADER}
                COMMAND api_gen ${EXT_DIR} ${EXT_API_HEADER}
                DEPENDS api_gen ${EXT_SOURCES}
                COMMENT "Generating ${EXT_NAME} extension API header"
                VERBATIM
            )
            
            # Add to list of all generated APIs
            list(APPEND ALL_EXTENSION_APIS ${EXT_API_HEADER})
            
            # Check if extension has custom CMakeLists.txt
            if(EXISTS ${EXT_DIR}/CMakeLists.txt)
                # Use custom build configuration
                add_subdirectory(${EXT_DIR} ${CMAKE_BINARY_DIR}/extensions/${EXT_NAME})
            else()
                # Simple extension - auto-generate build target
                set(LIB_NAME ${EXT_NAME}_extension)
                
                add_library(${LIB_NAME} STATIC ${EXT_SOURCES})
                
                target_include_directories(${LIB_NAME} PUBLIC 
                    ${SHARED_INCLUDE_DIR}
                )
                
                target_link_libraries(${LIB_NAME} PRIVATE shared)
                
                # Make extension depend on its generated API header
                add_custom_target(${LIB_NAME}_api DEPENDS ${EXT_API_HEADER})
                add_dependencies(${LIB_NAME} ${LIB_NAME}_api)
                
                list(APPEND ALL_EXTENSION_LIBS ${LIB_NAME})
            endif()
        else()
            message(STATUS "  Skipping ${EXT_NAME} - no .c files found")
        endif()
    endif()
endforeach()

# ============================================================================
# Generate plugin macros from all extension APIs
# ============================================================================
set(GENERATED_MACROS "${SHARED_INCLUDE_DIR}/plugin_macros_generated.h")

add_custom_command(
    OUTPUT ${GENERATED_MACROS}
    COMMAND macro_gen ${SHARED_INCLUDE_DIR} ${GENERATED_MACROS}
    DEPENDS macro_gen ${ALL_EXTENSION_APIS}
    COMMENT "Generating plugin macros from all extension APIs"
    VERBATIM
)

# ============================================================================
# Create targets that everything can depend on
# ============================================================================
add_custom_target(generate_extension_apis ALL 
    DEPENDS ${ALL_EXTENSION_APIS}
)

add_custom_target(generate_plugin_macros ALL 
    DEPENDS ${GENERATED_MACROS}
)

# Macros depend on APIs being generated first
add_dependencies(generate_plugin_macros generate_extension_apis)

# Export list of extension libraries for engine to link
set(FLIGHT_EXTENSION_LIBS ${ALL_EXTENSION_LIBS} CACHE INTERNAL "List of extension libraries")