cmake_minimum_required(VERSION 3.25)

set(CONAN_INSTALL_BUILD_CONFIGURATIONS "Release" CACHE STRING "" FORCE)
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_SOURCE_DIR}/conan_provider.cmake CACHE PATH "" FORCE)

project(Explosion)

option(BUILD_EDITOR "Build Explosion editor" ON)
option(BUILD_SAMPLE "Build samples" ON)

set(SUB_PROJECT_NAME "Explosion" CACHE STRING "" FORCE)
set(SUB_PROJECT_VERSION_MAJOR 0 CACHE STRING "" FORCE)
set(SUB_PROJECT_VERSION_MINOR 0 CACHE STRING "" FORCE)
set(SUB_PROJECT_VERSION_PATCH 1 CACHE STRING "" FORCE)
set(SUB_PROJECT_CMAKE_LIBS "ThirdParty/Registry.cmake" CACHE STRING "" FORCE)
set(USE_CONAN ON CACHE BOOL "" FORCE)

# Conan builds its dependencies as Release, so map the other configs onto Release imported locations. The trailing
# empty element keeps the configuration-less IMPORTED_LOCATION as a fallback; system find-modules such as FindOpenGL
# create UNKNOWN imported targets (OpenGL::OpenGL / OpenGL::GLX on Linux) that only set the suffix-less location, and
# without this fallback CMake reports "IMPORTED_LOCATION not set ... configuration Debug" at generate time.
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG "Release;" CACHE STRING "" FORCE)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Release;" CACHE STRING "" FORCE)
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL "Release;" CACHE STRING "" FORCE)

add_compile_definitions(BUILD_EDITOR=$<BOOL:${BUILD_EDITOR}>)

include(CMake/Common.cmake)
include(CMake/Target.cmake)
include(ThirdParty/Registry.cmake)

add_subdirectory(Engine)
add_subdirectory(Tool)
add_subdirectory(Sample)

if (${BUILD_EDITOR})
    add_subdirectory(Editor)
endif()
