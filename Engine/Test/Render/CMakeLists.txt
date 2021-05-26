file(GLOB RENDER_TEST_SOURCES *.cpp)
exp_add_test(
    NAME RenderTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${RENDER_TEST_SOURCES}
    LIBS gtest explosion
)
