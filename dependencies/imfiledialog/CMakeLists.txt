# ImFileDialog

# ImFileDialog lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(imfiledialog ${headers} ${srcs})

# Link against imgui and stb as they are required
target_link_libraries(imfiledialog imgui)
target_link_libraries(imfiledialog stb)