file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Render
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Render.Test
    SRC ${TEST_SOURCES}
    LIB Render
)
