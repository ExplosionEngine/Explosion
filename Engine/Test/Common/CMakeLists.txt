file(GLOB COMMON_TEST_SOURCES *.cpp)
add_executable(CommonTest ${COMMON_TEST_SOURCES})
target_link_libraries(CommonTest gtest explosion)
add_test(
    NAME CommonTest
    COMMAND CommonTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)