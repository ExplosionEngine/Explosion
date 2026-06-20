option(USE_UNITY_BUILD "Use unity build" ON)
option(EXPORT_COMPILE_COMMANDS "Whether to export all compile commands" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_UNITY_BUILD ${USE_UNITY_BUILD})
set(CMAKE_EXPORT_COMPILE_COMMANDS ${EXPORT_COMPILE_COMMANDS})

# Static libraries such as Common get linked into the engine's shared libraries, so on ELF platforms every object must be
# position-independent or the shared link fails with "relocation ... can not be used when making a shared object".
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/Installed CACHE PATH "" FORCE)
endif()

add_compile_definitions(
    BUILD_CONFIG_DEBUG=$<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,1,0>
    PLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,1,0>
    PLATFORM_LINUX=$<IF:$<PLATFORM_ID:Linux>,1,0>
    PLATFORM_MACOS=$<IF:$<PLATFORM_ID:Darwin>,1,0>
    COMPILER_MSVC=$<IF:$<CXX_COMPILER_ID:MSVC>,1,0>
    COMPILER_APPLE_CLANG=$<IF:$<CXX_COMPILER_ID:AppleClang>,1,0>
    COMPILER_GCC=$<IF:$<CXX_COMPILER_ID:GNU>,1,0>
)

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    add_compile_options(/bigobj)
    add_compile_definitions(
        _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS=1
        WIN32_LEAN_AND_MEAN
        NOMINMAX=1
    )
endif ()

# This project and its downstream consumers reference Qt only through the versioned Qt6:: targets. Suppressing the
# versionless Qt:: aliases stops the single-config Qt host tools from triggering "IMPORTED_LOCATION not set ...
# configuration <cfg>" errors in the IDE file-API codemodel on non-Release builds, where they only carry a Release
# import while Debug/RelWithDebInfo/MinSizeRel are mapped onto it.
set(QT_NO_CREATE_VERSIONLESS_TARGETS TRUE)
