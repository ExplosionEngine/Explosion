file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Rendering.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Render Core
)

file(GLOB SHARED_SOURCES SharedSrc/*.cpp)
AddLibrary(
    NAME Rendering
    TYPE SHARED
    SRC ${SHARED_SOURCES}
    LIB Rendering.Static
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Rendering.Test
    SRC ${TEST_SOURCES}
    LIB RHI Rendering.Static
)
