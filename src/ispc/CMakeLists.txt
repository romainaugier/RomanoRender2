# ISPC

# Ispc lib

file(GLOB_RECURSE ispc_files *.ispc)
set(ispc_names "")
set(ispc_outputs "")

foreach(ispc_file ${ispc_files})
    get_filename_component(ispc_filename ${ispc_file} NAME)
    string(REPLACE ".ispc" "" filename ${ispc_filename})
    list(APPEND ispc_names ${filename})

    add_custom_command(
        OUTPUT ${PROJECT_BINARY_DIR}/${filename}.o ${filename}.h
        COMMAND ${ISPC_EXECUTABLE} ${ispc_file} -h ${filename}.h -o ${filename}.o --target=avx2 -O3 
        )

    list(APPEND ispc_outputs ${PROJECT_BINARY_DIR}/${filename}.o)
endforeach(ispc_file)

add_library(ispc_library ${ispc_outputs})
set_target_properties(ispc_library
                      PROPERTIES
                      LINKER_LANGUAGE C)       