file(GLOB sources Src/*.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(platform_sources Src/Platform/Win32Surface.cpp)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_sources Src/Platform/MacosSurface.mm)
    set(platform_ext_libs
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

exp_add_library(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${sources} ${platform_sources}
    PUBLIC_INC Include
    PUBLIC_LIB RHI ${platform_ext_libs}
    PUBLIC_MERGE_LIB VulkanSDK spirv-cross GPUOpen::VulkanMemoryAllocator
)

# .mm files can not perform unity build with .cpp files
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set_target_properties(
        RHI-Vulkan PROPERTIES
        UNITY_BUILD OFF)
endif()
