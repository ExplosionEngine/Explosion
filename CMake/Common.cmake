option(BUILD_EDITOR "Build Explosion editor" ON)
option(CI "Build in CI" OFF)
option(USE_UNITY_BUILD "Use unity build" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_UNITY_BUILD ${USE_UNITY_BUILD})

get_cmake_property(GENERATOR_IS_MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)
if (${GENERATOR_IS_MULTI_CONFIG})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/Engine/Binaries)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/Engine/Binaries)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
else ()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/Engine/Binaries)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/Engine/Binaries)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
endif ()

add_definitions(-DENGINE_CMAKE_SOURCE_DIRECTORY="${CMAKE_SOURCE_DIR}")
add_definitions(-DENGINE_CMAKE_BINARY_DIRECTORY="${CMAKE_BINARY_DIR}")

add_definitions(-DBUILD_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>)

add_definitions(-DPLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,1,0>)
add_definitions(-DPLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,1,0>)
add_definitions(-DPLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,1,0>)

add_definitions(-DCOMPILER_MSVC=$<IF:$<CXX_COMPILER_ID:MSVC>,1,0>)
add_definitions(-DCOMPILER_APPLE_CLANG=$<IF:$<CXX_COMPILER_ID:AppleClang>,1,0>)
add_definitions(-DCOMPILER_GCC=$<IF:$<CXX_COMPILER_ID:GNU>,1,0>)

add_definitions(-DBUILD_EDITOR=$<BOOL:BUILD_EDITOR>)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    string(REGEX REPLACE ".*MacOSX([0-9]+\\.[0-9]+).*" "\\1" MACOS_SDK_VERSION ${CMAKE_OSX_SYSROOT})
    add_definitions(-DMACOS_SDK_VERSION=${MACOS_SDK_VERSION})
endif ()

if (${MSVC})
    add_compile_options(/bigobj)
    add_definitions(-D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1)
    add_definitions(-DNOMINMAX=1)
endif ()
