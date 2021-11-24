# Renderer

# Lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(renderer ${headers} ${srcs})

# Link with everything we need
target_link_libraries(renderer shading)