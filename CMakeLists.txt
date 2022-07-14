cmake_minimum_required(VERSION 3.20)
project(Explosion)

include(ExternalProject)
include(GenerateExportHeader)

include(CMake/Common.cmake)
include(CMake/Target.cmake)
include(CMake/ThirdParty.cmake)

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
