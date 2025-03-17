add_subdirectory(Source)

function(GetEngineShaderResources)
    cmake_parse_arguments(PARAMS "" "NAME" "OUTPUT" ${ARGN})

    file(GLOB_RECURSE ENGINE_SHADERS ${CMAKE_SOURCE_DIR}/Engine/Shader/*.es*)
    foreach (SHADER ${ENGINE_SHADERS})
        get_filename_component(SHADER_ABSOLUTE ${SHADER} ABSOLUTE)
        string(REPLACE ${CMAKE_SOURCE_DIR}/Engine/Shader ../Shader/Engine COPY_DST ${SHADER_ABSOLUTE})
        list(APPEND RESULT ${SHADER}->${COPY_DST})
    endforeach ()

    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()
