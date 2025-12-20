cmake_minimum_required(VERSION 3.25)

set(CONAN_INSTALL_BUILD_CONFIGURATIONS "Release" CACHE STRING "" FORCE)
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_SOURCE_DIR}/conan_provider.cmake CACHE PATH "" FORCE)

project(Explosion)

option(BUILD_EDITOR "Build Explosion editor" ON)
option(CI "Build in CI" OFF)

set(SUB_PROJECT_NAME "Engine" CACHE STRING "" FORCE)
set(ENGINE_SUB_PROJECT_NAME "Engine" CACHE STRING "" FORCE)

set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Release" CACHE STRING "" FORCE)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Release" CACHE STRING "" FORCE)
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "Release" CACHE STRING "" FORCE)

add_definitions(-DBUILD_EDITOR=$<BOOL:BUILD_EDITOR>)

include(CMake/Common.cmake)
include(CMake/Target.cmake)

add_subdirectory(ThirdParty)
add_subdirectory(Engine)
add_subdirectory(Tool)
add_subdirectory(Sample)

if (${BUILD_EDITOR})
    add_subdirectory(Editor)
endif()
