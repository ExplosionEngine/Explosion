file(GLOB COMMON_TEST_SOURCES *.cpp)
exp_add_test(
    NAME CommonTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${COMMON_TEST_SOURCES}
    LIBS gtest explosion
)
