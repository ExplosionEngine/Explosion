file(GLOB SOURCES Src/*.cpp Src/*.mm)

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
    PRIVATE_INC ${VulkanSDK_INC}
    PRIVATE_LINK ${VulkanSDK_LINK}
    LIB RHI ${VulkanSDK_VULKAN_LIB} ${PLATFORM_EXT_LIBS}
)
