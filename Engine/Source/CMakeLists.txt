add_subdirectory(Common)
add_subdirectory(RHI)
add_subdirectory(Mirror)
add_subdirectory(Render)
add_subdirectory(Runtime)
add_subdirectory(Rendering)
add_subdirectory(Launcher)
add_subdirectory(Example)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_subdirectory(RHI-DirectX12)
endif()

if(APPLE)
    add_subdirectory(RHI-Metal)
endif ()

add_subdirectory(RHI-Vulkan)
