# Embree algorithms

# Lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(algorithms ${headers} ${srcs})