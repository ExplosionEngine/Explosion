include(ExternalProject)
include(GenerateExportHeader)

option(USE_UNITY_BUILD "Use unity build" ON)
option(EXPORT_COMPILE_COMMANDS "Whether to export all compile commands" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_UNITY_BUILD ${USE_UNITY_BUILD})
set(CMAKE_EXPORT_COMPILE_COMMANDS ${EXPORT_COMPILE_COMMANDS})

get_cmake_property(generator_is_multi_config GENERATOR_IS_MULTI_CONFIG)
if (${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT})
    # TODO support multi config generator for CMAKE_INSTALL_PREFIX
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Install CACHE PATH "" FORCE)
endif()

add_definitions(-DBUILD_CONFIG_DEBUG=$<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,1,0>)

add_definitions(-DPLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,1,0>)
add_definitions(-DPLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,1,0>)
add_definitions(-DPLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,1,0>)

add_definitions(-DCOMPILER_MSVC=$<IF:$<CXX_COMPILER_ID:MSVC>,1,0>)
add_definitions(-DCOMPILER_APPLE_CLANG=$<IF:$<CXX_COMPILER_ID:AppleClang>,1,0>)
add_definitions(-DCOMPILER_GCC=$<IF:$<CXX_COMPILER_ID:GNU>,1,0>)

if (${MSVC})
    add_compile_options(/bigobj /MD)
    add_definitions(-D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX=1)
endif ()
