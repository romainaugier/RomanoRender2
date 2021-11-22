# Src

# Get all subdirectories needed
add_subdirectory(common)
add_subdirectory(ispc)
add_subdirectory(utils)
add_subdirectory(scene)
add_subdirectory(shading)
add_subdirectory(renderer)
add_subdirectory(application)

# Executable
add_executable(${PROJECT_NAME} main.cpp)

# Link with the application lib, as it's already linked with everything else

# Make sure we compile with CPP 17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)