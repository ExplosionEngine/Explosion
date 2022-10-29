file(GLOB SOURCES Src/*)
file(GLOB INCLUDES Include/*)

find_library(METAL_LIB Metal)
find_library(CORE_GFX CoreGraphics)
find_library(QUARTZ_CORE QuartzCore)
find_library(COCOA Cocoa)

set(PLATFORM_EXT_LIBS
    ${METAL_LIB}
    ${CORE_GFX}
    ${QUARTZ_CORE}
    ${COCOA})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    file(GLOB PLATFORM_SRC Src/Platform/Mac/*)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    file(GLOB PLATFORM_SRC Src/Platform/IOS/*)
endif()

AddLibrary(
    NAME RHI-Metal
    TYPE SHARED
    SRC ${SOURCES} ${INCLUDES} ${PLATFORM_SRC} ${PLATFORM_SOURCES}
    PUBLIC_INC Include
    LIB RHI spirv-cross ${PLATFORM_EXT_LIBS}
)
