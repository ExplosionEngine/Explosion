cmake_minimum_required(VERSION 3.15)
project(Explosion)

set(CMAKE_CXX_STANDARD 17)

include(ExternalProject)
include(GenerateExportHeader)
include(CMake/Target.cmake)
include(CMake/ThirdParty.cmake)

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
