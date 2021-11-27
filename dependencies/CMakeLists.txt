# Dependencies

# GLFW
include_directories(glfw/include)

# ImGui
add_subdirectory(imgui)
include_directories(imgui)

# imnodes
add_subdirectory(imnodes)
include_directories(imnodes)

# imfiledialog
add_subdirectory(imfiledialog)
include_directories(imfiledialog)