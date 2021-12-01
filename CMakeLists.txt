# GLOBAL #

cmake_minimum_required(VERSION 3.15)

# Vcpkg
if(DEFINED CMAKE_TOOLCHAIN_FILE)
	if(EXISTS "${CMAKE_TOOLCHAIN_FILE}")
		message(STATUS "Vcpkg has been found")
	else()
		message(FATAL_ERROR "Vcpkg can't be found, CMake will exit")
	endif()
else()
	if(EXISTS "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
		message(STATUS "Vcpkg has been found")	
		set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "CMAKE Toolchain" FORCE)
	else()
		message(FATAL_ERROR "Vcpkg can't be found, CMake will exit")
	endif()
endif()

# Vcpkg triplet
if(WIN32)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL 4)
		message(FATAL_ERROR "VCPKG : windows:x86 triplet is not supported, CMake will exit")
	elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
		set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "VCPKG Triplet" FORCE)
	endif()
elseif(UNIX)
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL 4)
		message(FATAL_ERROR "VCPKG : linux:x86 triplet is not supported, CMake will exit.")
	elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
		set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "VCPKG Triplet" FORCE)
	endif()
endif()

# ISPC
if(DEFINED ISPC_EXECUTABLE)
	if(EXISTS "${ISPC_EXECUTABLE}")
		message(STATUS "Found ISPC executable : ${ISPC_EXECUTABLE}")
	else()
		message(FATAL_ERROR "ISPC can't be found, CMake will exit")
	endif()
else()
	find_program(ISPC_EXECUTABLE ispc)
	if(NOT ISPC_EXECUTABLE)
		message(FATAL_ERROR "ISPC can't be found, CMake will exit")
	else()
		message(STATUS "Found ISPC executable : ${ISPC_EXECUTABLE}")
	endif()
endif()

# Linux stdlib
if(UNIX)
    set(CMAKE_CXX_STANDARD_LIBRARIES "-ldl")
endif()

# Project
project(RomanoRender2)

# CPP Standard
set(CMAKE_CXX_STANDARD 17)

# Compile and link options
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS -D__AVX2__)
	set(MSVC_COMPILER_FLAGS "/fp:fast /Ox /Os /Oy /GT /GL /Oi /arch:AVX2 /Zi /Gm- /Zc:inline /Qpar")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "${MSVC_COMPILER_FLAGS}")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_options(-fopenmp -O3 -mavx2 -mf16c -fpermissive)
	add_link_options(-lstdc++fs)
else()
	message(FATAL_ERROR "Your compiler is not supported yet, CMake will exit.")
endif()

# Embree
find_package(embree 3 REQUIRED)
include_directories(${EMBREE_INCLUDE_DIRS})

# OpenImageIO
find_package(OpenImageIO REQUIRED)
include_directories(${OpenImageIO_INCLUDE_DIRS})

# Tbb
find_package(tbb REQUIRED)

# OSL
# Pugixml required by osl
find_package(pugixml REQUIRED) 

set(OSL_INCLUDE_DIRS dependencies/osl/include)
include_directories(${OSL_INCLUDE_DIRS})

# ImGui / ImNodes / ImFileDialog / GLFW
set(IMGUI_INCLUDES dependencies/imgui
				   dependencies/imnodes
				   dependencies/imfiledialog
				   dependencies/glfw/include)
include_directories(${IMGUI_INCLUDES})

# Subdirs
add_subdirectory(dependencies)

# Src
include_directories(src)
add_subdirectory(src)

