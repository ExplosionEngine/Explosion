file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME Core
    TYPE SHARED
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Common
    PUBLIC_MERGE_LIB clipp::clipp
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Core.Test
    SRC ${test_sources}
    LIB Core
)
