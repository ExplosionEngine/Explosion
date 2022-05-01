file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Common
)

function(AddRHISample)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;SHADER" ${ARGN})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PLATFORM_EXT_LIBS
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreFoundation")
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(RUNTIME_DEPS ${VulkanSDK_DXC_DLL})
    endif()

    AddExecutable(
        NAME ${PARAMS_NAME}
        SRC ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SRC}
        INC Sample ${glfw_INC} ${GLM_INC} ${clipp_INC} ${VulkanSDK_INC}
        LINK ${glfw_LINK} ${VulkanSDK_LINK}
        LIB RHI ${glfw_LIB} ${PLATFORM_EXT_LIBS} ${VulkanSDK_DXC_LIB}
        RUNTIME_DEP ${RUNTIME_DEPS}
    )
    add_custom_command(
        TARGET ${PARAMS_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${PARAMS_SHADER} $<TARGET_FILE_DIR:${PARAMS_NAME}>
    )

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        add_dependencies(${PARAMS_NAME} RHI-DirectX12 RHI-Vulkan)
    else ()
        add_dependencies(${PARAMS_NAME} RHI-Vulkan)
    endif ()
endfunction()

AddRHISample(
    NAME RHI-Triangle
    SRC Sample/Triangle.cpp
    SHADER Sample/Triangle.hlsl
)
