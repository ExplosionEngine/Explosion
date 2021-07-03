cmake_minimum_required(VERSION 3.15)
project(Explosion)

set(CMAKE_CXX_STANDARD 17)

include(cmake/Explosion.cmake)

option(EXP_VERBOSE_INFO "verbose info for cmake debug" ON)
option(EXP_BUILD_TEST "build test" ON)
option(EXP_BUILD_SAMPLES "build samples" ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(GEN_PRE_DIR ${CMAKE_BINARY_DIR}/Gen/ApiHeader)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
if (${CMAKE_GENERATOR} MATCHES "^Visual Studio .*$")
    set(PROJECT_RUNTIME_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE})
else()
    set(PROJECT_RUNTIME_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
endif()

include(GenerateExportHeader)
# TODO using api define in each module
# set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

include_directories(${GEN_PRE_DIR})

if (${EXP_BUILD_TEST})
    message("-- enable testing")
    enable_testing()
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DEXPLOSION_DEBUG)
endif()

add_subdirectory(3rd)
add_subdirectory(Engine)
add_subdirectory(Modules)
