file(GLOB_RECURSE SOURCES Src/* Include/*)
exp_add_library(
    NAME CommonBase
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME CommonBaseTest
    TYPE STATIC
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS CommonBase ${googletest_LIBS}
)
