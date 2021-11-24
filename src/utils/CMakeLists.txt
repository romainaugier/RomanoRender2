# Utils

# Utils lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(utils ${headers} ${srcs})

# Link with scene as its needed
target_link_libraries(utils scene)