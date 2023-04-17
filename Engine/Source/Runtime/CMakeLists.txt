file(GLOB_RECURSE SOURCES Src/*.cpp)
AddLibrary(
    NAME Runtime
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    REFLECT Include
    LIB EnTT GLM Mirror
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Runtime.Test
    SRC ${TEST_SOURCES}
    LIB Runtime
)
