# Embree simd

# Lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(simd ${headers} ${srcs})

# Link against sys lib as its needed
target_link_libraries(simd sys)