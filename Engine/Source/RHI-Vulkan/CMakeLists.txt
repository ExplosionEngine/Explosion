file(GLOB SOURCES Src/*.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM_SOURCES Src/Platform/Win32Surface.cpp)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_SOURCES Src/Platform/MacosSurface.mm)
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC Include
    LIB RHI VulkanSDK spirv-cross VulkanMemoryAllocator ${PLATFORM_EXT_LIBS}
)

# .mm files can not perform unity build with .cpp files
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set_target_properties(
        RHI-Vulkan PROPERTIES
        UNITY_BUILD OFF)
endif()
