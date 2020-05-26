cmake_minimum_required(VERSION 3.15)
project(explosion)

set(CMAKE_CXX_STANDARD 20)

set(GLFW_VERSION 3.3.2)

option(USE_GLFW_LIB "use glfw lib" ON)

if (USE_GLFW_LIB)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "build shared GLFW lib")
    add_subdirectory(3rd/glfw-${GLFW_VERSION})
endif()