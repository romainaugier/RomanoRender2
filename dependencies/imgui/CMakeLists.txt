# ImGui

# ImGui lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(imgui ${headers} ${srcs})

# Link against glfw as it's our platform
if(WIN32)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-vc2019-x64/glfw3.lib)
	endif()
elseif(UNIX)
	if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		target_link_libraries(imgui ${CMAKE_SOURCE_DIR}/dependencies/glfw/lib-linux/libglfw.so)
	endif()
endif()