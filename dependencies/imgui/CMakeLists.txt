# ImGui

# ImGui lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(imgui ${headers} ${srcs})

# Link against glfw/glew as it's our platform, and copy dlls
if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/glfw3.lib)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/glew32.lib)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/glew32s.lib)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/opengl32.lib)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/freeglut.lib)
	endif()
elseif(UNIX)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-linux/libglfw.so)
	endif()
endif()