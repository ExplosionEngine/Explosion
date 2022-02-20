add_subdirectory(Common)
add_subdirectory(Console)
add_subdirectory(RHI)
add_subdirectory(RenderGraph)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_subdirectory(RHI-DirectX12)
endif ()
add_subdirectory(RHI-Vulkan)