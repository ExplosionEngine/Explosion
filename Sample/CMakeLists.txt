function(AddSample)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;INC;SHADER;IMAGE;MODEL" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(PLATFORM_DEP_TARGET RHI-DirectX12 RHI-Vulkan)
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

    list(APPEND PATHS "${CMAKE_SOURCE_DIR}/Engine/Shader/Platform.h->../Test/Sample/ShaderInclude/Platform.h")
    list(APPEND PATHS "${CMAKE_SOURCE_DIR}/Engine/Shader/Platform.h->../Test/Sample/ShaderInclude/Platform.esh")

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${PARAMS_SRC}
        INC ${PARAMS_INC}
        LIB Sample-Base
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
    NAME Sample-Base
    SRC ${SOURCES}
    PUBLIC_INC Base
    LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb assimp-lib Rendering.Static
)

file(GLOB SOURCES RHI-Triangle/*.cpp)
AddSample(
    NAME RHISample-Triangle
    SRC ${SOURCES}
    INC RHI-Triangle
    SHADER RHI-Triangle/Triangle.hlsl
)

file(GLOB SOURCES RHI-TexSampling/*.cpp)
AddSample(
    NAME RHISample-TexSampling
    SRC ${SOURCES}
    INC RHI-TexSampling
    SHADER RHI-TexSampling/TexSampling.hlsl
    IMAGE RHI-TexSampling/Awesomeface.png
)

file(GLOB SOURCES RHI-SSAO/*.cpp)
AddSample(
    NAME RHISample-SSAO
    SRC ${SOURCES}
    INC RHI-SSAO
    SHADER
        RHI-SSAO/Shader/Gbuffer.hlsl
        RHI-SSAO/Shader/SSAO.hlsl
        RHI-SSAO/Shader/Blur.hlsl
        RHI-SSAO/Shader/Composition.hlsl
    MODEL
        RHI-SSAO/Model/Voyager.gltf
)

file(GLOB SOURCES Rendering-Triangle/*.cpp)
AddSample(
    NAME RenderingSample-Triangle
    SRC ${SOURCES}
    INC Rendering-Triangle
    SHADER
        Rendering-Triangle/Triangle.esl
)
