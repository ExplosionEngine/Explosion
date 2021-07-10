file(GLOB_RECURSE ECS_TEST_SOURCES *.cpp)
exp_add_test(
    NAME ECSTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${ECS_TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS Engine ${googletest_LIBS}
)
