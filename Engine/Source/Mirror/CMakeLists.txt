file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME Mirror
    TYPE SHARED
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Common
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Mirror.Test
    SRC ${test_sources}
    LIB Mirror
    INC Test
    REFLECT Test
)
