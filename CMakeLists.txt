cmake_minimum_required(VERSION 3.25)
project(Explosion)

option(BUILD_EDITOR "Build Explosion editor" ON)
option(CI "Build in CI" OFF)

set(SUB_PROJECT_NAME "Engine" CACHE STRING "" FORCE)
set(ENGINE_SUB_PROJECT_NAME "Engine" CACHE STRING "" FORCE)

add_definitions(-DBUILD_EDITOR=$<BOOL:BUILD_EDITOR>)

include(CMake/Common.cmake)
include(CMake/Target.cmake)
include(CMake/ThirdParty.cmake)

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
add_subdirectory(Tool)
add_subdirectory(Sample)

if (${BUILD_EDITOR})
    add_subdirectory(Editor)
endif()
