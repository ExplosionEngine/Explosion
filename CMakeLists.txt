cmake_minimum_required(VERSION 3.15)
project(Explosion)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(PROJECT_RUNTIME_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_subdirectory(3rd)
add_subdirectory(Engine)
add_subdirectory(Application)
add_subdirectory(Samples)
