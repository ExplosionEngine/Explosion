# EnTT
Add3rdHeaderOnlyPackage(
    NAME EnTT
    PLATFORM All
    VERSION 3.8.1
    INCLUDE $<SOURCE_DIR>/single_include
)

# GLM
Add3rdHeaderOnlyPackage(
    NAME GLM
    PLATFORM All
    VERSION 0.9.9.8
    INCLUDE $<SOURCE_DIR>
)

# DirectX 12 Headers
Add3rdBinaryPackage(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    INCLUDE $<SOURCE_DIR>/include
)

# VulkanSDK
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Windows
    VERSION 1.2.198.1
    INCLUDE $<SOURCE_DIR>/Include
    LINK $<SOURCE_DIR>/Lib
    LIB vulkan-1
)
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Darwin
    VERSION 1.2.198.1
    INCLUDE $<SOURCE_DIR>/macOS/include
    LINK $<SOURCE_DIR>/macOS/lib
    LIB vulkan.1
)

# DXC
# Windows uses standalone package, macOS uses lib in VulkanSDK
Add3rdBinaryPackage(
    NAME dxc
    PLATFORM Windows
    VERSION 1.6.2112
    INCLUDE $<SOURCE_DIR>/inc
    LINK $<SOURCE_DIR>/lib
    LIB dxcompiler
    RUNTIME_DEP $<SOURCE_DIR>/bin/x64/dxcompiler.dll $<SOURCE_DIR>/bin/x64/dxil.dll
)
Add3rdAliasPackage(
    NAME dxc
    PLATFORM Darwin
    LIB dxcompiler
)

# GLFW
Add3rdCMakeProject(
    NAME glfw
    PLATFORM All
    VERSION 3.3.6
    CMAKE_ARG -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB glfw3
)

# clipp
Add3rdHeaderOnlyPackage(
    NAME clipp
    PLATFORM All
    VERSION forked-f7cffbd61a7b64189d6573e8c3848d094c35168a
    INCLUDE $<SOURCE_DIR>/include
)

# DebugBreak
Add3rdHeaderOnlyPackage(
    NAME debugbreak
    PLATFORM All
    VERSION 1.0
    INCLUDE $<SOURCE_DIR>
)

# LLVM-Clang
Add3rdBinaryPackage(
    NAME llvm-clang
    PLATFORM Windows
    VERSION 14.0.3
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB libclang
    RUNTIME_DEP $<SOURCE_DIR>/bin/libclang.dll
)
Add3rdBinaryPackage(
    NAME llvm-clang
    PLATFORM Darwin
    VERSION 14.0.3
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB clang
)

# Google Test
Add3rdCMakeProject(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB gtest$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,> gmock$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
)

# spirv-cross
Add3rdCMakeProject(
    NAME spirv-cross
    PLATFORM All
    VERSION 1.1.5
    CMAKE_ARG -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_C_API=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB spirv-cross-core$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
)

# TaskFlow
Add3rdHeaderOnlyPackage(
    NAME taskflow
    PLATFORM All
    VERSION 3.4.0
    INCLUDE $<SOURCE_DIR>
)

# CityHash
Add3rdCMakeProject(
    NAME cityhash
    PLATFORM All
    VERSION forked-4c7281e82789cf0c4ef54770dd085d06c274aaf0
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB city
)
