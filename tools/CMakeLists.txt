project(flight_tools LANGUAGES C)

# Build API generator
add_executable(api_gen 
    api_gen.c
)

# Build macro generator
add_executable(macro_gen 
    macro_gen.c
)

# These are standalone tools with no dependencies
# They get built first, then used by extensions