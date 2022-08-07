file(GLOB SOURCES Src/*.cpp Src/*.mm)
file(GLOB TEST_SOURCES Test/*.cpp)

if (APPLE)
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI spirv-cross VulkanSDK ${PLATFORM_EXT_LIBS}
)
