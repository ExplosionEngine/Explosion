file(GLOB RENDER_TEST_SOURCES *.cpp)
add_executable(RenderTest ${RENDER_TEST_SOURCES})
target_link_libraries(RenderTest gtest explosion)
add_test(
    NAME RenderTest
    COMMAND RenderTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)