file(GLOB TRIANGLE_SOURCES main.cpp)
add_executable(1-Triangle ${TRIANGLE_SOURCES})
target_link_libraries(1-Triangle explosion application)
