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
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${S}->${S}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(I ${PARAMS_IMAGE})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${I}->${I}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(M ${PARAMS_MODEL})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${M}->${M}")
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
    IMAGE TexSampling/awesomeface.png
)

AddRHISample(
    NAME RHI-SSAO
    SRC
        SSAO/GLTFParser.cpp
        SSAO/SSAOApplication.cpp
        SSAO/Renderer.cpp
    SHADER
        SSAO/shaders/gbufferPs.hlsl
        SSAO/shaders/gbufferVs.hlsl
        SSAO/shaders/fullscreenVs.hlsl
        SSAO/shaders/ssaoPs.hlsl
        SSAO/shaders/blurPs.hlsl
        SSAO/shaders/compositionPs.hlsl
        SSAO/Shaders/common.h
    MODEL SSAO/models/voyager.gltf
)
