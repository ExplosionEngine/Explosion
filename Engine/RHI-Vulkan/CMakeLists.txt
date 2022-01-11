file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    PRIVATE_INC ${VulkanSDK_INC}
    PRIVATE_LINK ${VulkanSDK_LINK}
    LIB RHI ${VulkanSDK_VULKAN_LIB}
)
