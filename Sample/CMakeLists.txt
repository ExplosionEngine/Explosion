function(add_sample)
    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC SHADER IMAGE MODEL)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(platform_dep_target RHI-DirectX12 RHI-Vulkan)
    else()
        set(platform_dep_target RHI-Vulkan)
    endif()

    foreach(s ${arg_SHADER})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${s}->../Test/Sample/${s}")
        list(APPEND paths ${path})
    endforeach()

    foreach(i ${arg_IMAGE})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${i}->../Test/Sample/${i}")
        list(APPEND paths ${path})
    endforeach()

    foreach(m ${arg_MODEL})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${m}->../Test/Sample/${m}")
        list(APPEND paths ${path})
    endforeach()

    list(APPEND paths "${CMAKE_SOURCE_DIR}/${ENGINE_SUB_PROJECT_NAME}/Shader/Platform.esh->../Test/Sample/ShaderInclude/Platform.esh")

    exp_add_executable(
        SAMPLE
        NAME ${arg_NAME}
        SRC ${arg_SRC}
        INC ${arg_INC}
        LIB Sample-Base
        DEP_TARGET ${platform_dep_target}
        RES ${paths}
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
exp_add_library(
    NAME Sample-Base
    SRC ${SOURCES}
    PUBLIC_INC Base
    PUBLIC_LIB RHI ${PLATFORM_EXT_LIBS} Render.Static
    PUBLIC_MERGE_LIB glfw clipp stb assimp-lib
    NOT_INSTALL
)

file(GLOB SOURCES RHI-Triangle/*.cpp)
add_sample(
    NAME RHISample-Triangle
    SRC ${SOURCES}
    INC RHI-Triangle
    SHADER RHI-Triangle/Triangle.esl
)

file(GLOB SOURCES RHI-TexSampling/*.cpp)
add_sample(
    NAME RHISample-TexSampling
    SRC ${SOURCES}
    INC RHI-TexSampling
    SHADER RHI-TexSampling/TexSampling.esl
    IMAGE RHI-TexSampling/Awesomeface.png
)

file(GLOB SOURCES RHI-SSAO/*.cpp)
add_sample(
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
add_sample(
    NAME RHISample-ParallelCompute
    SRC ${SOURCES}
    SHADER RHI-ParallelCompute/Compute.esl
)

file(GLOB SOURCES Rendering-Triangle/*.cpp)
add_sample(
    NAME RenderingSample-Triangle
    SRC ${SOURCES}
    INC Rendering-Triangle
    SHADER
        Rendering-Triangle/Triangle.esl
)

file(GLOB SOURCES Rendering-BaseTexture/*.cpp)
add_sample(
        NAME Rendering-BaseTexture
        SRC ${SOURCES}
        INC Rendering-BaseTexture
        SHADER Rendering-BaseTexture/BaseTexture.esl
        IMAGE Rendering-BaseTexture/Awesomeface.png
)

