# ISPC

# Ispc lib

file(GLOB_RECURSE ispc_files *.ispc)

message(${ispc_files})

add_library(ispc_library ${ispc_files})
set_target_properties(ispc_library
                      PROPERTIES
                      LINKER_LANGUAGE C)       