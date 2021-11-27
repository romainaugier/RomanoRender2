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

# Make sure we compile with CPP 17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Compile definition to avoid autolink with tbb
target_compile_definitions(${PROJECT_NAME} PUBLIC __TBB_NO_IMPLICIT_LINKAGE)

if(WIN32)
elseif(UNIX)
endif()

# Copy needed binaries (from OSL)
set(OSL_BINS dependencies/osl/bin/oslcomp.dll 
             dependencies/osl/bin/oslexec.dll 
             dependencies/osl/bin/oslnoise.dll 
             dependencies/osl/bin/oslquery.dll 
             dependencies/osl/bin/osl.imageio.dll
             dependencies/osl/bin/OpenColorIO.dll 
             dependencies/osl/bin/oslc.exe)

foreach(binary_file ${OSL_BINS})
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${binary_file} ${CMAKE_BINARY_DIR}
    )
endforeach(binary_file)