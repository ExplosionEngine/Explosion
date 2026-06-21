cmake_minimum_required(VERSION {{cmakeMinVersion}})

project({{projectName}})

set(ENGINE_ROOT "" CACHE STRING "Root directory of engine (where ExplosionConfig.cmake inside), support source build engine or packed engine.")
set(SUB_PROJECT_NAME "{{projectName}}" CACHE STRING "Name of project." FORCE)
if ("${ENGINE_ROOT}" STREQUAL "")
    message(FATAL_ERROR "ENGINE_ROOT not set, please add -DENGINE_ROOT=/path/to/engine/root to cmake arguments.")
endif ()

set(CMAKE_PREFIX_PATH ${ENGINE_ROOT})
find_package(Explosion REQUIRED)

exp_add_executable(
    NAME Main
    SRC Game/Src/Main.cpp
    LIB Explosion::Core
)
