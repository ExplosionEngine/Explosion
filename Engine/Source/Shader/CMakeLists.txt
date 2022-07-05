if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_LIBS VulkanSDK)
endif()

file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Shader
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI GLM ${PLATFORM_LIBS} dxc
)

file(GLOB SOURCES Test/*.cpp)
AddTest(
    META
    NAME Shader.Test
    SRC ${SOURCES}
    INC Test
    LIB Shader
)
