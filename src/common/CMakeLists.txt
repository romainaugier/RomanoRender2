# Common (Embree)

file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

# Common lib
add_library(common ${headers} ${srcs})

# Some compile definitions
target_compile_definitions(common PUBLIC __SSE4_2__)
target_compile_definitions(common PUBLIC __AVX2__)
target_compile_definitions(common PUBLIC _CRT_SECURE_NO_WARNINGS)