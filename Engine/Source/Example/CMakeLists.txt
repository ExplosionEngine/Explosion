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

file(GLOB SOURCES Src/*.cpp)

AddExecutable(
    SAMPLE
    NAME SSAO
    SRC ${SOURCES}
    INC Include
    LIB RHI glfw GLM clipp ${PLATFORM_EXT_LIBS} Render stb tinyglft
    DEP_TARGET ${PLATFORM_DEP_TARGET}
)

add_custom_command(
    TARGET SSAO PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Resources ${CMAKE_BINARY_DIR}/Bin
)