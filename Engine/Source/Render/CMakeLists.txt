if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_libs VulkanSDK)
endif()

file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME Render.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    PUBLIC_LIB Core RHI
    PUBLIC_MERGE_LIB dxc spirv-cross ${platform_libs}
)

file(GLOB SHARED_SOURCES SharedSrc/*.cpp)
exp_add_library(
    NAME Render
    TYPE SHARED
    SRC ${SHARED_SOURCES}
    PUBLIC_LIB Render.Static
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Render.Test
    SRC ${TEST_SOURCES}
    LIB RHI Render.Static
    DEP_TARGET RHI-Dummy
)
