# TODO for dxc, replace this with real dxc lib someday ?
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_LIBS VulkanSDK)
endif()

file(GLOB_RECURSE SOURCES Src/*.cpp)
AddLibrary(
    NAME Render
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI ${PLATFORM_LIBS} dxc spirv-cross
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Render.Test
    SRC ${TEST_SOURCES}
    LIB Render
)
