function(add_sample)
    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC SHADER IMAGE MODEL)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT BUILD_SAMPLE)
        return()
    endif ()

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

    list(APPEND paths "${CMAKE_SOURCE_DIR}/Engine/Shader/Platform.esh->../Test/Sample/ShaderInclude/Platform.esh")

    exp_add_executable(
        NAME ${arg_NAME}
        FOLDER Sample
        SRC ${arg_SRC}
        INC ${arg_INC}
        LIB Sample-Base
        DEP_TARGET ${platform_rhi_targets}
        RES ${paths}
        NOT_INSTALL
    )
endfunction()

if (APPLE)
    set(platform_ext_libs
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

file(GLOB sources Base/*.cpp)
exp_add_library(
    NAME Sample-Base
    SRC ${sources}
    PUBLIC_INC Base
    PUBLIC_LIB RHI ${platform_ext_libs} Render.Static glfw clipp::clipp stb::stb assimp::assimp
    NOT_INSTALL
)

file(GLOB sources RHI-Triangle/*.cpp)
add_sample(
    NAME RHISample-Triangle
    SRC ${sources}
    INC RHI-Triangle
    SHADER RHI-Triangle/Triangle.esl
)

file(GLOB sources RHI-TexSampling/*.cpp)
add_sample(
    NAME RHISample-TexSampling
    SRC ${sources}
    INC RHI-TexSampling
    SHADER RHI-TexSampling/TexSampling.esl
    IMAGE RHI-TexSampling/Awesomeface.png
)

file(GLOB sources RHI-SSAO/*.cpp)
add_sample(
    NAME RHISample-SSAO
    SRC ${sources}
    INC RHI-SSAO
    SHADER
        RHI-SSAO/Shader/Gbuffer.esl
        RHI-SSAO/Shader/SSAO.esl
        RHI-SSAO/Shader/Blur.esl
        RHI-SSAO/Shader/Composition.esl
    MODEL
        RHI-SSAO/Model/Voyager.gltf
)

file(GLOB sources RHI-ParallelCompute/*.cpp)
add_sample(
    NAME RHISample-ParallelCompute
    SRC ${sources}
    SHADER RHI-ParallelCompute/Compute.esl
)

file(GLOB sources Rendering-Triangle/*.cpp)
add_sample(
    NAME RenderingSample-Triangle
    SRC ${sources}
    INC Rendering-Triangle
    SHADER
        Rendering-Triangle/Triangle.esl
)

file(GLOB sources Rendering-BaseTexture/*.cpp)
add_sample(
    NAME RenderingSample-BaseTexture
    SRC ${sources}
    INC Rendering-BaseTexture
    SHADER Rendering-BaseTexture/BaseTexture.esl
    IMAGE Rendering-BaseTexture/Awesomeface.png
)

file(GLOB SOURCES Rendering-SSAO/*.cpp)
add_sample(
    NAME RenderingSample-SSAO
    SRC ${SOURCES}
    INC Rendering-SSAO
    SHADER
        Rendering-SSAO/Shader/Gbuffer.esl
        Rendering-SSAO/Shader/SSAO.esl
        Rendering-SSAO/Shader/Blur.esl
        Rendering-SSAO/Shader/Composition.esl
    MODEL
        Rendering-SSAO/Model/Voyager.gltf
)

