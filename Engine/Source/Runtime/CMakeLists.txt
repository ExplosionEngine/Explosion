file(GLOB_RECURSE SOURCES Src/*.cpp)
AddLibrary(
    NAME Runtime
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    REFLECT Include
    LIB Core Mirror assimp-lib Render EnTT
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Runtime.Test
    SRC ${TEST_SOURCES}
    LIB Runtime
    INC Test
    REFLECT Test
)
