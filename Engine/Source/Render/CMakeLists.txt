if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_LIBS VulkanSDK)
endif()

file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME Render.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Core RHI ${PLATFORM_LIBS} dxc spirv-cross
)

file(GLOB SHARED_SOURCES SharedSrc/*.cpp)
exp_add_library(
    NAME Render
    TYPE SHARED
    SRC ${SHARED_SOURCES}
    LIB Render.Static
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Render.Test
    SRC ${TEST_SOURCES}
    LIB RHI Render.Static
)
