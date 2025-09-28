file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Common
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB debugbreak cityhash taskflow rapidjson
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Common.Test
    INC Test
    SRC ${TEST_SOURCES}
)
