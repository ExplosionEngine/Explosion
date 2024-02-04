file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Rendering
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Render Core
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Rendering.Test
    SRC ${TEST_SOURCES}
    LIB RHI Rendering
)
