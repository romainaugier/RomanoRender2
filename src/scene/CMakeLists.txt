# Scene

# Scene lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(scene ${headers} ${srcs})

# Link against embree and maths library
target_link_libraries(scene embree)
target_link_libraries(scene maths)