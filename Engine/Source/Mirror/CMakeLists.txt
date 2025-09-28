file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME Mirror
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Mirror.Test
    SRC ${TEST_SOURCES}
    LIB Mirror
    INC Test
    REFLECT Test
)
