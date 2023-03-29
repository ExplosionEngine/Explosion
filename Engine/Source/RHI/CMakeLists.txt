file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
)

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
    else()
        set(PLATFORM_DEP_TARGET RHI-Metal RHI-Vulkan)
    endif()

    foreach(S ${PARAMS_SHADER})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${S}->Shader/${S}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(I ${PARAMS_IMAGE})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${I}->Image/${I}")
        list(APPEND PATHS ${path})
    endforeach()

    foreach(M ${PARAMS_MODEL})
        set(path "${CMAKE_CURRENT_SOURCE_DIR}/${M}->Model/${M}")
        list(APPEND PATHS ${path})
    endforeach()

    AddExecutable(
        SAMPLE
        NAME ${PARAMS_NAME}
        SRC ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SRC}
        INC Sample
        LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb
        DEP_TARGET ${PLATFORM_DEP_TARGET}
        RES ${PATHS}
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

