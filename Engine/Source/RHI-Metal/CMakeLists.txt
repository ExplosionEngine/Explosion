file(GLOB SOURCES Src/*)

find_library(METAL_LIB Metal)
find_library(CORE_GFX CoreGraphics)
find_library(QUARTZ_CORE QuartzCore)

set(PLATFORM_EXT_LIBS
    ${METAL_LIB}
    ${CORE_GFX}
    ${QUARTZ_CORE})

AddLibrary(
    NAME RHI-Metal
    TYPE SHARED
    SRC ${SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC Include
    LIB RHI spirv-cross ${PLATFORM_EXT_LIBS}
)
