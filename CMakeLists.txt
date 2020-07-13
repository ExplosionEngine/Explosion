cmake_minimum_required(VERSION 3.15)
project(explosion)

set(CMAKE_CXX_STANDARD 20)

# version configurations
set(GLFW_VERSION 3.3.2)

# build GLFW
set(GLFW_HOME 3rd/glfw-${GLFW_VERSION})
include_directories(${GLFW_HOME}/include)
set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared GLFW lib")
add_subdirectory(${GLFW_HOME})

# build GLAD
set(GLAD_HOME 3rd/glad)
include_directories(${GLAD_HOME}/include)
add_library(glad SHARED 3rd/glad/src/glad.c)

# build engine
include_directories(include)
aux_source_directory(src/Engine ENGINE_SOURCE)
aux_source_directory(src/Render RENDER_SOURCE)
add_library(explosion SHARED ${ENGINE_SOURCE} ${RENDER_SOURCE})
target_link_libraries(explosion glfw glad)

# link OpenGL
if (APPLE)
    target_link_libraries(explosion "-framework OpenGL")
endif()

# build example
if (APPLE)
    set(EXAMPLE_HOME example/macos)
    add_executable(example ${EXAMPLE_HOME}/main.cpp)
endif()

# link example
target_link_libraries(example explosion)