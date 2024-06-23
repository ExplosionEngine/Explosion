function(AddRHISample)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;INC;SHADER;IMAGE;MODEL" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(PLATFORM_DEP_TARGET RHI-DirectX12 RHI-Vulkan)
    else()
        set(PLATFORM_DEP_TARGET RHI-Vulkan)
    endif()

    foreach(S ${PARAMS_SHADER})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${S}->../Test/Sample/RHI/${S}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(I ${PARAMS_IMAGE})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${I}->../Test/Sample/RHI/${I}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(M ${PARAMS_MODEL})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${M}->../Test/Sample/RHI/${M}")
        list(APPEND PATHS ${path})
    endforeach()

    list(APPEND PATHS "${CMAKE_SOURCE_DIR}/Engine/Shader/Platform.h->../Test/Sample/RHI/ShaderInclude/Platform.h")

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${PARAMS_SRC}
        INC ${PARAMS_INC}
        LIB RHISample-Base
        DEP_TARGET ${PLATFORM_DEP_TARGET}
        RES ${PATHS}
    )
endfunction()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

file(GLOB SOURCES Base/*.cpp)
Addlibrary(
    NAME RHISample-Base
    SRC ${SOURCES}
    PUBLIC_INC Base
    LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb assimp-lib
)

file(GLOB SOURCES Triangle/*.cpp)
AddRHISample(
    NAME RHISample-Triangle
    SRC ${SOURCES}
    INC Triangle
    SHADER Triangle/Triangle.hlsl
)

file(GLOB SOURCES TexSampling/*.cpp)
AddRHISample(
    NAME RHISample-TexSampling
    SRC ${SOURCES}
    INC TexSampling
    SHADER TexSampling/TexSampling.hlsl
    IMAGE TexSampling/Awesomeface.png
)

file(GLOB SOURCES SSAO/*.cpp)
AddRHISample(
    NAME RHISample-SSAO
    SRC ${SOURCES}
    INC SSAO
    SHADER
        SSAO/Shader/Gbuffer.hlsl
        SSAO/Shader/SSAO.hlsl
        SSAO/Shader/Blur.hlsl
        SSAO/Shader/Composition.hlsl
    MODEL SSAO/Model/Voyager.gltf
)

AddRHISample(
    NAME RHISample-ParallelCompute
    SRC ParallelCompute/ParallelCompute.cpp
    SHADER ParallelCompute/Compute.hlsl
)
