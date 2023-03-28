file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
)

function(AddRHISample)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;SHADER;IMAGE" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PLATFORM_EXT_LIBS
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreFoundation")
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(PLATFORM_DEP_TARGET RHI-DirectX12 RHI-Vulkan)
    else()
        set(PLATFORM_DEP_TARGET RHI-Metal RHI-Vulkan)
    endif()

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SRC}
        INC Sample
        LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb
        DEP_TARGET ${PLATFORM_DEP_TARGET}
        RES
            ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SHADER}->Shader/${PARAMS_SHADER}
            ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_IMAGE}->Image/${PARAMS_IMAGE}
    )
endfunction()

AddRHISample(
    NAME RHI-Triangle
    SRC Sample/Triangle.cpp
    SHADER Sample/Triangle.hlsl
)

AddRHISample(
    NAME RHI-TexSampling
    SRC Sample/TexSampling.cpp
    SHADER Sample/TexSampling.hlsl
    IMAGE Sample/awesomeface.png
)

