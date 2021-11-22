# STB

# STB lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(stb ${headers} ${srcs})