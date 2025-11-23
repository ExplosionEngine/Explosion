add_subdirectory(Source)

function(get_engine_shader_resources)
    set(options "")
    set(singleValueArgs OUTPUT)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    file(GLOB_RECURSE engine_shaders ${CMAKE_SOURCE_DIR}/${ENGINE_SUB_PROJECT_NAME}/Shader/*.es*)
    foreach (shader ${engine_shaders})
        get_filename_component(shader_absolute ${shader} ABSOLUTE)
        string(REPLACE ${CMAKE_SOURCE_DIR}/${ENGINE_SUB_PROJECT_NAME}/Shader ../Shader/${ENGINE_SUB_PROJECT_NAME} copy_dst ${shader_absolute})
        list(APPEND result ${shader}->${copy_dst})
    endforeach ()

    set(${arg_OUTPUT} ${result} PARENT_SCOPE)
endfunction()
