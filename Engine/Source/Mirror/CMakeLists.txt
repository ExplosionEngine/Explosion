file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Mirror
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Mirror.Test
    SRC ${TEST_SOURCES}
    LIB Mirror
)
