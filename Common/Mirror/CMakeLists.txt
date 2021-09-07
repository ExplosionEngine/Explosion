file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Mirror
    TYPE STATIC
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME MirrorTest
    WORKING_DIRECTORY ${PROJECT_RUNTIME_DIRECTORY}
    SRCS ${TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS Mirror ${googletest_LIBS}
)
