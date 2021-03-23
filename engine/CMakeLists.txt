# Vulkan ------------------------------------------------------------------------------
if (NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "found no env named VULKAN_SDK")
endif()
include_directories("$ENV{VULKAN_SDK}/Include")

# Engine ------------------------------------------------------------------------------
option(VK_VALIDATION_LAYER_ENABLED "option for vulkan validation layer" ON)

include_directories(include)
file(GLOB_RECURSE ENGINE_SOURCES src/**/*.cpp)

add_library(engine ${ENGINE_SOURCES})

if (WIN32)
    target_link_libraries(
        engine
        "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib"
    )
else()
    target_link_libraries(
        engine
        "$ENV{VULKAN_SDK}/lib/libvulkan.1.dylib"
    )
endif()
