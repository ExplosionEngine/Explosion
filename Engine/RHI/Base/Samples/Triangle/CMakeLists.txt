file(GLOB TRIANGLE_SOURCES Main.cpp)
exp_add_executable(
    NAME RHI-Triangle
    SRCS ${TRIANGLE_SOURCES}
    LIBS Engine SampleApplication FileSystem RHI
)
add_custom_command(
    TARGET RHI-Triangle PRE_BUILD
    COMMAND glslc -fshader-stage=vertex ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/RHI-Triangle-Vertex.spv
    COMMAND glslc -fshader-stage=fragment ${CMAKE_CURRENT_SOURCE_DIR}/Fragment.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/RHI-Triangle-Fragment.spv
)
