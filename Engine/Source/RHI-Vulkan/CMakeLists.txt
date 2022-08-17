file(GLOB SOURCES Src/*.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM_SOURCES Src/Platform/Win32Surface.cpp)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_SOURCES Src/Platform/MacosSurface.mm)
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

AddLibrary(
    NAME RHI-Vulkan
    TYPE SHARED
    SRC ${SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC Include
    LIB RHI spirv-cross VulkanSDK ${PLATFORM_EXT_LIBS}
)
