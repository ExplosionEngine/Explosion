file(GLOB TRIANGLE_SOURCES Main.cpp)
exp_add_executable(
    NAME 1-Triangle
    SRCS ${TRIANGLE_SOURCES}
    LIBS explosion application
)
add_custom_command(
    TARGET 1-Triangle PRE_BUILD
    COMMAND glslc -fshader-stage=vertex ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/1-Triangle-Vertex.spv
    COMMAND glslc -fshader-stage=fragment ${CMAKE_CURRENT_SOURCE_DIR}/Fragment.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/1-Triangle-Fragment.spv
)
