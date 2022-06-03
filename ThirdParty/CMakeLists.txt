# EnTT
Add3rdHeaderOnlyPackage(
    NAME EnTT
    PLATFORM All
    VERSION 3.8.1
    HASH 77e55bfc4774c1be3a5a6a7a35b321a20c5dff37e688af3895e20cc56011e5f4
    INCLUDE $<SOURCE_DIR>/single_include
)

# GLM
Add3rdHeaderOnlyPackage(
    NAME GLM
    PLATFORM All
    VERSION 0.9.9.8
    HASH 29876d3440386464c781853caf73eb49eaa1b04a5e648a0cff92381c5e9e69b4
    INCLUDE $<SOURCE_DIR>
)

# Meta
Add3rdHeaderOnlyPackage(
    NAME Meta
    PLATFORM All
    VERSION 1.4.9
    HASH 9cd04221c4865fdef37f8a92646087ec777ded6b1ff1a5e932cd0cd72b175eba
    INCLUDE $<SOURCE_DIR>/src
)

# DirectX 12 Headers
Add3rdBinaryPackage(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    HASH 32a28d1172eb5d6b1574a2ba51d628ba446f0e8a1d3291b5a4903a661c430738
    INCLUDE $<SOURCE_DIR>/include
)

# VulkanSDK
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Windows
    HASH 006dfa3a63644fed382089402f64095ee9ca8ce3ec3107b80aae6e8941de9e9b
    VERSION 1.2.198.1
    INCLUDE $<SOURCE_DIR>/Include
    LINK $<SOURCE_DIR>/Lib
    LIB
        vulkan-1
        spirv-cross-core
)
Add3rdBinaryPackage(
    NAME VulkanSDK
    PLATFORM Darwin
    HASH 6394c8e1f355820a8f6808bab2d87d518ee284e42b761da24e516f8ab0e2f75d
    VERSION 1.2.198.1
    INCLUDE $<SOURCE_DIR>/macOS/include
    LINK $<SOURCE_DIR>/macOS/lib
    LIB
        vulkan.1
        spirv-cross-core
)

# DXC
# Windows uses standalone package, macOS uses lib in VulkanSDK
Add3rdBinaryPackage(
    NAME dxc
    PLATFORM Windows
    HASH 389901e244e15852adc25197f01bdd1860d4d15ebd3a9f683b869721a06e15f9
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
    HASH e2eb069a70da0fc862af4d14e6a80748883f12e20bdce101c028968165d9c688
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
    HASH 26c5dbff7310d153125b0f9ea8c587d9b5255fa14c61800f93cb358779cb503f
    INCLUDE $<SOURCE_DIR>/include
)

# DebugBreak
Add3rdHeaderOnlyPackage(
    NAME debugbreak
    PLATFORM All
    VERSION 1.0
    HASH 609053a8a508063ec4312d6991de39767eb12cbf36dd048e02a6a4e41ce13319
    INCLUDE $<SOURCE_DIR>
)

# LLVM
Add3rdBinaryPackage(
    NAME LLVM
    PLATFORM Windows
    VERSION 14.0.3
    HASH 6639417bf5e54b584819818b91dd21031d64b7600fba7d309e88033fe25e4b09
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB libclang
    RUNTIME_DEP $<SOURCE_DIR>/bin/libclang.dll
)
Add3rdBinaryPackage(
    NAME LLVM
    PLATFORM Darwin
    VERSION 14.0.3
    HASH c28df5decc632b2dd1c13e2e30f27e9a5934db5de365e7d31e2a964b479e928c
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB clang
)

# Google Test
Add3rdCMakeProject(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    HASH d15f6f53d20ed98e1ee4669663b4af7d072c3397ee6d8125de1fd6922115c6a4
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB gtest$<IF:$<CONFIG:Debug>,d,> gmock$<IF:$<CONFIG:Debug>,d,>
)
