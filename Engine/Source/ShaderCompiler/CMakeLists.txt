if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_LIBS VulkanSDK)
endif()

file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME ShaderCompiler
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI GLM ${PLATFORM_LIBS} dxc
)
