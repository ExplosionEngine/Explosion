cmake_minimum_required(VERSION 3.25)

get_cmake_property(generator_is_multi_config GENERATOR_IS_MULTI_CONFIG)
if (${generator_is_multi_config})
    # TODO only install release config, need some deals in exp_add_library
    set(CONAN_INSTALL_BUILD_CONFIGURATIONS "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif ()
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_SOURCE_DIR}/conan_provider.cmake CACHE PATH "" FORCE)

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
