# Embree maths

# Maths lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(maths ${headers} ${srcs})

# Link against simd as its needed
target_link_libraries(maths simd)