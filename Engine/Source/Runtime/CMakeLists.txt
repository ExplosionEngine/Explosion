file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Runtime
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    REFLECT Include
    LIB Core Mirror assimp-lib Render
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Runtime.Test
    SRC ${TEST_SOURCES}
    LIB Runtime
    INC Test
    REFLECT Test
)
