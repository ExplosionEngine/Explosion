file(GLOB sources Src/*.cpp)

if (WIN32)
    set(platform_sources Src/Platform/Win32Surface.cpp)
elseif (APPLE)
    set(platform_sources Src/Platform/MacosSurface.mm)
    set(platform_ext_libs molten-vk::molten-vk "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation")
endif()

exp_add_library(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${sources} ${platform_sources}
    PUBLIC_INC Include
    PUBLIC_LIB RHI ${platform_ext_libs} Vulkan::Headers Vulkan::Loader vulkan-validationlayers::vulkan-validationlayers spirv-cross::spirv-cross GPUOpen::VulkanMemoryAllocator
)

# .mm files can not perform unity build with .cpp files
if (APPLE)
    set_target_properties(
        RHI-Vulkan PROPERTIES
        UNITY_BUILD OFF)
endif()
