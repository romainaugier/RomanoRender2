# ImNodes

# ImNodes lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(imnodes ${headers} ${srcs})

# Link against ImGui as it is required
target_link_libraries(imnodes imgui)