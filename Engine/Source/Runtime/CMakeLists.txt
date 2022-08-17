file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Runtime
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB EnTT GLM
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Runtime.Test
    SRC ${TEST_SOURCES}
    LIB Runtime
)
