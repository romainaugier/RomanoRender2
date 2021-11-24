# Shading

# Lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(shading ${headers} ${srcs})

# Link with utils and osl
target_link_libraries(shading utils)
target_link_libraries(shading osl)