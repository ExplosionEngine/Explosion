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

    list(APPEND PATHS "${CMAKE_SOURCE_DIR}/Engine/Shader/Platform.esh->../Test/Sample/ShaderInclude/Platform.esh")

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${PARAMS_SRC}
        INC ${PARAMS_INC}
        LIB Sample-Base
        DEP_TARGET ${PLATFORM_DEP_TARGET}
        RES ${PATHS}
        NOT_INSTALL
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
    LIB RHI glfw clipp ${PLATFORM_EXT_LIBS} Render.Static stb assimp-lib
    NOT_INSTALL
)

file(GLOB SOURCES RHI-Triangle/*.cpp)
AddSample(
    NAME RHISample-Triangle
    SRC ${SOURCES}
    INC RHI-Triangle
    SHADER RHI-Triangle/Triangle.esl
)

file(GLOB SOURCES RHI-TexSampling/*.cpp)
AddSample(
    NAME RHISample-TexSampling
    SRC ${SOURCES}
    INC RHI-TexSampling
    SHADER RHI-TexSampling/TexSampling.esl
    IMAGE RHI-TexSampling/Awesomeface.png
)

file(GLOB SOURCES RHI-SSAO/*.cpp)
AddSample(
    NAME RHISample-SSAO
    SRC ${SOURCES}
    INC RHI-SSAO
    SHADER
        RHI-SSAO/Shader/Gbuffer.esl
        RHI-SSAO/Shader/SSAO.esl
        RHI-SSAO/Shader/Blur.esl
        RHI-SSAO/Shader/Composition.esl
    MODEL
        RHI-SSAO/Model/Voyager.gltf
)

file(GLOB SOURCES RHI-ParallelCompute/*.cpp)
AddSample(
    NAME RHISample-ParallelCompute
    SRC ${SOURCES}
    SHADER RHI-ParallelCompute/Compute.esl
)

file(GLOB SOURCES Rendering-Triangle/*.cpp)
AddSample(
    NAME RenderingSample-Triangle
    SRC ${SOURCES}
    INC Rendering-Triangle
    SHADER
        Rendering-Triangle/Triangle.esl
)

file(GLOB SOURCES Rendering-BaseTexture/*.cpp)
AddSample(
        NAME Rendering-BaseTexture
        SRC ${SOURCES}
        INC Rendering-BaseTexture
        SHADER Rendering-BaseTexture/BaseTexture.esl
        IMAGE Rendering-BaseTexture/Awesomeface.png
)

