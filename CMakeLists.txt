cmake_minimum_required(VERSION 3.15)
project(Explosion)

set(CMAKE_CXX_STANDARD 17)

if (NOT DEFINED ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GEN_PRE_DIR ${CMAKE_BINARY_DIR}/Gen/ApiHeader)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(PROJECT_RUNTIME_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

include(GenerateExportHeader)
# [TODO] using api define in each module
# set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

include_directories(${GEN_PRE_DIR})

# options
option(BUILD_TEST "Build Test" ON)
if (${BUILD_TEST})
    enable_testing()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
if(${BUILD_TYPE_LOWER} STREQUAL "debug")
    add_definitions(-DEXPLOSION_DEBUG)
endif()

add_subdirectory(3rd)
add_subdirectory(Engine)
add_subdirectory(Application)
add_subdirectory(Samples)
add_subdirectory(Plugins)
