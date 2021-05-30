file(GLOB SAMPLE_SOURCES Main.cpp)

# TODO
find_package(Vulkan)
if (${Vulkan_FOUND})
    message("-- found Vulkan library")
else()
    message(FATAL_ERROR "-- can not find the Vulkan lib, make sure you have installed the Vulkan SDK")
endif()
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(../)

exp_add_executable(
    NAME ParticleSample
    SRCS ${SAMPLE_SOURCES}
    LIBS explosion ParticleSystem application
)
add_custom_command(
    TARGET ParticleSample PRE_BUILD
    COMMAND glslc -fshader-stage=vertex ${CMAKE_CURRENT_SOURCE_DIR}/Vertex.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/ParticleSpriteVertex.spv
    COMMAND glslc -fshader-stage=fragment ${CMAKE_CURRENT_SOURCE_DIR}/Fragment.glsl -o ${PROJECT_RUNTIME_DIRECTORY}/ParticleSpriteFragment.spv
)
