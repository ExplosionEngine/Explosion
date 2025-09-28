file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME Core
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB clipp
)

file(GLOB TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME Core.Test
    SRC ${TEST_SOURCES}
    LIB Core
)
