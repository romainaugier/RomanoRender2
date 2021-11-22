# Dependencies

# GLFW
include_directories(glfw/include)

# ImGui
add_subdirectory(imgui)
include_directories(imgui)

# STB
add_subdirectory(stb)
include_directories(stb)

# imnodes
add_subdirectory(imnodes)
include_directories(imnodes)

# imfiledialog
add_subdirectory(imfiledialog)
include_directories(imfiledialog)