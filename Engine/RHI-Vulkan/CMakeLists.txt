file(GLOB SOURCES Src/*.cpp Src/*.mm)
file(GLOB TEST_SOURCES Test/*.cpp)

if (APPLE)
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

AddLibrary(
    NAME RHI-Vulkan.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include ${VulkanSDK_INC}
    PRIVATE_LINK ${VulkanSDK_LINK}
    LIB RHI ${VulkanSDK_VULKAN_LIB} ${PLATFORM_EXT_LIBS}
)

AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC RHIVulkan.cpp
    PRIVATE_LINK ${VulkanSDK_LINK}
    LIB RHI-Vulkan.Static
)

AddTest(
    NAME RHI-VulkanTEST
    SRC ${TEST_SOURCES}
    INC
        Include
        ${VulkanSDK_INC}
    LINK ${VulkanSDK_LINK}
    LIB RHI-Vulkan.Static
)
