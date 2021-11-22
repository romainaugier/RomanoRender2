# Utils

# Utils lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(utils ${headers} ${srcs})

# Link with 