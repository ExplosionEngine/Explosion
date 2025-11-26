file(GLOB_RECURSE sources Src/*.cpp)
exp_add_library(
    NAME Runtime
    TYPE SHARED
    SRC ${sources}
    PUBLIC_INC Include
    REFLECT Include
    PUBLIC_LIB Core Mirror Render
    PUBLIC_MERGE_LIB assimp-lib
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Runtime.Test
    SRC ${test_sources}
    LIB Runtime
    INC Test
    REFLECT Test
    DEP_TARGET RHI-Dummy
)
