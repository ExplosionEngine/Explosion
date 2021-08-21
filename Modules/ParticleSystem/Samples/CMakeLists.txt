file(GLOB SAMPLE_SOURCES Main.cpp)

find_package(Vulkan REQUIRED)

exp_add_executable(
    NAME ParticleSample
    SRCS ${SAMPLE_SOURCES}
    INC_DIRS ${Vulkan_INCLUDE_DIRS} ..
    LIBS Engine RHI ParticleSystem SampleApplication FileSystem
)
add_custom_command(
    TARGET ParticleSample PRE_BUILD
    COMMAND glslc -fshader-stage=vertex ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/ParticleSpriteVertex.spv
    COMMAND glslc -fshader-stage=fragment ${CMAKE_CURRENT_SOURCE_DIR}/Fragment.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/ParticleSpriteFragment.spv
)
