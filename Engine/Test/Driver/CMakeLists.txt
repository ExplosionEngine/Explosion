file(GLOB_RECURSE DRIVER_TEST_SOURCES *.cpp)
add_executable(DriverTest ${DRIVER_TEST_SOURCES})
target_link_libraries(DriverTest gtest explosion)
add_test(
    NAME DriverTest
    COMMAND DriverTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
