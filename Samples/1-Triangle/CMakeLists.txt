file(GLOB TRIANGLE_SOURCES Main.cpp)
add_executable(1-Triangle ${TRIANGLE_SOURCES})
target_link_libraries(1-Triangle explosion application)
add_custom_command(
    TARGET 1-Triangle PRE_BUILD
    COMMAND glslc -fshader-stage=vertex ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/1-Triangle-Vertex.spv
    COMMAND glslc -fshader-stage=fragment ${CMAKE_CURRENT_SOURCE_DIR}/Fragment.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/1-Triangle-Fragment.spv
)
