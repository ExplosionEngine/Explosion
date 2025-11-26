file(GLOB_RECURSE sources Src/*.cpp)
exp_add_library(
    NAME Common
    TYPE STATIC
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_MERGE_LIB rapidjson debugbreak cityhash taskflow
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Common.Test
    INC Test
    SRC ${test_sources}
    LIB Common
)
