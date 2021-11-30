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
target_link_libraries(${PROJECT_NAME} app)
target_link_libraries(${PROJECT_NAME} ispc_library)

# Make sure we compile with CPP 17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Compile definition to avoid autolink with tbb
target_compile_definitions(${PROJECT_NAME} PUBLIC __TBB_NO_IMPLICIT_LINKAGE)

if(WIN32)
elseif(UNIX)
endif()

# Copy needed binaries (from OSL)
set(OSL_BINS ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/oslcomp.dll 
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/oslexec.dll 
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/oslnoise.dll 
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/oslquery.dll 
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/osl.imageio.dll
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/OpenColorIO.dll 
             ${CMAKE_SOURCE_DIR}/dependencies/osl/bin/oslc.exe)

foreach(binary_file ${OSL_BINS})
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${binary_file} ${CMAKE_CURRENT_BINARY_DIR}/Release
    )
endforeach(binary_file)

# Copy needed binaries (from GLEW/GLFW)
set(GL_DLLS ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/freeglut.dll
			${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/glew32.dll)

foreach(binary_file ${GL_DLLS})
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${binary_file} ${CMAKE_CURRENT_BINARY_DIR}/Release
    )
endforeach()