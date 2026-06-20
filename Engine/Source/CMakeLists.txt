if (BUILD_TEST)
    add_subdirectory(Test)
endif()

if (BUILD_BENCHMARK)
    add_subdirectory(Benchmark)
endif()

add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(Mirror)

add_subdirectory(RHI)
add_subdirectory(RHI-Dummy)
add_subdirectory(RHI-Vulkan)
if (WIN32)
    add_subdirectory(RHI-DirectX12)
endif()

add_subdirectory(Render)
add_subdirectory(Runtime)
add_subdirectory(Launch)
