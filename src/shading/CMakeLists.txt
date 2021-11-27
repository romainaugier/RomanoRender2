# Shading

# Lib
file(GLOB_RECURSE srcs *.cpp)
file(GLOB_RECURSE headers *.h)

add_library(shading ${headers} ${srcs})

# Link with utils and osl
target_link_libraries(shading utils)

set(OSL_LIBS ${CMAKE_SOURCE_DIR}/dependencies/osl/lib/osl.imageio.lib
             ${CMAKE_SOURCE_DIR}/dependencies/osl/lib/oslcomp.lib
             ${CMAKE_SOURCE_DIR}/dependencies/osl/lib/oslexec.lib
             ${CMAKE_SOURCE_DIR}/dependencies/osl/lib/oslnoise.lib
             ${CMAKE_SOURCE_DIR}/dependencies/osl/lib/oslquery.lib)

target_link_libraries(shading ${OSL_LIBS})