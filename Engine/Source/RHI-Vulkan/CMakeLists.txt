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
    PUBLIC_INC Include
    LIB RHI VulkanSDK ${PLATFORM_EXT_LIBS}
)

AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC RHIVulkan.cpp
    LIB RHI-Vulkan.Static
)

# TODO fix this unit test
#AddTest(
#    NAME RHI-VulkanTest
#    SRC ${TEST_SOURCES}
#    LIB RHI-Vulkan.Static
#)
