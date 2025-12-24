cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(glfw REQUIRED)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_ext_libs
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreFoundation")
endif()

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw::glfw ${platform_ext_libs})
