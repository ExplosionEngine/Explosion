file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Core
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB clipp
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Core.Test
    SRC ${TEST_SOURCES}
    LIB Core
)
