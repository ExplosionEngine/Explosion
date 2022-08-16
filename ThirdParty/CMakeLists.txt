# EnTT
Add3rdHeaderOnlyPackage(
    NAME EnTT
    PLATFORM All
    VERSION 3.8.1
    HASH abe24e5da599ef9f172c25f68f3b777dd404896b06e500742ecc88bbfef2216c
    INCLUDE $<SOURCE_DIR>/single_include
)

# GLM
Add3rdHeaderOnlyPackage(
    NAME GLM
    PLATFORM All
    VERSION 0.9.9.8
    HASH f78f0cf73e5601536203e0098c4e611d1c37d3df60b3209e70a10e3abd3bbe58
    INCLUDE $<SOURCE_DIR>
)

# DirectX 12 Headers
Add3rdBinaryPackage(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    HASH 0313c5145214dda2a183af97212878482aafbb2433d4f0ec0a2efeec7cc9ef2a
    INCLUDE $<SOURCE_DIR>/include
)

# VulkanSDK
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Windows
    VERSION 1.3.216.0
    HASH cf52875b10661939e484135c33331e2f883e6530c578f5be8698f3afb8e39c13
    INCLUDE $<SOURCE_DIR>/Include
    LINK $<SOURCE_DIR>/Lib
    LIB vulkan-1
)
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Darwin
    VERSION 1.3.216.0
    HASH 9315737591b3d62275418b724ef03dcf8686a2e748044487677c80e4d7e47ba9
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
    HASH b8d8e5cd03234e477bc3fe5d4258652a7fb61916141660408ee4c54a4f1eb939
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
    HASH d3a7807f604b0f92af7606b2018a007892bb8de0db5efb99e26fb1f41b828da4
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
    HASH 59db8b8586381652b5e0ae629a9d32e0e443428bc4d94af091b9525a62d167fb
    INCLUDE $<SOURCE_DIR>/include
)

# DebugBreak
Add3rdHeaderOnlyPackage(
    NAME debugbreak
    PLATFORM All
    VERSION 1.0
    HASH 16e35a330a9927181ed2e03a92efe6d39670b33c1bdf61ab88d29673ef3a89d4
    INCLUDE $<SOURCE_DIR>
)

# LLVM-Clang
Add3rdBinaryPackage(
    NAME llvm-clang
    PLATFORM Windows
    VERSION 14.0.3
    HASH ba82f63eb5ac4df1925f8268e2b5c54c0542e7cf51e140266e69b7c4aea43c53
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB libclang
    RUNTIME_DEP $<SOURCE_DIR>/bin/libclang.dll
)
Add3rdBinaryPackage(
    NAME llvm-clang
    PLATFORM Darwin
    VERSION 14.0.3
    HASH a35022cc58a358756e68dc10e6fcae4b7108d92c2d699c2769cc015b3013fcdf
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB clang
)

# Google Test
Add3rdCMakeProject(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    HASH bb1146f051868868b96fa83069d4d05f0e05216b5c3f697859c591fd73b93a86
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB gtest$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,> gmock$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
)

# spirv-cross
Add3rdCMakeProject(
    NAME spirv-cross
    PLATFORM All
    VERSION 1.1.5
    HASH db9126514071a49c15b545ddff0b67efd45898b5f9462e7796d303392c076913
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
    HASH 774e23d5b6be17a5482d70ffd76ba4ccfd8821c86b7c3f0bcdcd5d96bc264041
    INCLUDE $<SOURCE_DIR>
)

# CityHash
Add3rdCMakeProject(
    NAME cityhash
    PLATFORM All
    VERSION forked-4c7281e82789cf0c4ef54770dd085d06c274aaf0
    HASH 2c606b415dbdc5115cf11d2ed2ab9f72c9b3db86655571f1593948b88187a64e
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB city
)
