function(AddRHISample)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;SHADER;IMAGE;MODEL" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PLATFORM_EXT_LIBS
                "-framework Cocoa"
                "-framework IOKit"
                "-framework CoreFoundation")
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(PLATFORM_DEP_TARGET RHI-DirectX12 RHI-Vulkan)
        #    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        #        set(PLATFORM_DEP_TARGET RHI-Metal RHI-Vulkan)
    else()
        set(PLATFORM_DEP_TARGET RHI-Vulkan)
    endif()

    foreach(S ${PARAMS_SHADER})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${S}->../Test/Sample/${S}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(I ${PARAMS_IMAGE})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${I}->../Test/Sample/${I}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(M ${PARAMS_MODEL})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${M}->../Test/Sample/${M}")
        list(APPEND PATHS ${path})
    endforeach()

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SRC}
        INC ${CMAKE_CURRENT_SOURCE_DIR}
        LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb assimp-lib
        DEP_TARGET ${PLATFORM_DEP_TARGET}
        RES ${PATHS}
    )
endfunction()

AddRHISample(
    NAME RHI-Triangle
    SRC Triangle/Triangle.cpp
    SHADER Triangle/Triangle.hlsl
)

AddRHISample(
    NAME RHI-TexSampling
    SRC TexSampling/TexSampling.cpp
    SHADER TexSampling/TexSampling.hlsl
    IMAGE TexSampling/Awesomeface.png
)

AddRHISample(
    NAME RHI-SSAO
    SRC
        SSAO/GLTFParser.cpp
        SSAO/SSAOApplication.cpp
    SHADER
        SSAO/Shader/Gbuffer.hlsl
        SSAO/Shader/SSAO.hlsl
        SSAO/Shader/Blur.hlsl
        SSAO/Shader/Composition.hlsl
        SSAO/Shader/Common.h
    MODEL SSAO/Model/Voyager.gltf
)

AddRHISample(
    NAME RHI-ParallelCompute
    SRC ParallelCompute/ParallelCompute.cpp
    SHADER ParallelCompute/Compute.hlsl
)
