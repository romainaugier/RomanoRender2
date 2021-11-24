# App

# App lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(app ${headers} ${srcs})

# Link against dependencies (we don't link against imgui as it is already linked with imnodes and imfiledialog)
target_link_libraries(app imnodes)
target_link_libraries(app imfiledialog)
target_link_libraries(app renderer)