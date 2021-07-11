file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME JobSystem
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include ${taskflow_INCS}
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME JobSystemTest
    TYPE STATIC
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS JobSystem ${googletest_LIBS}
)
