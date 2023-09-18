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
Add3rdHeaderOnlyPackage(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    HASH 0313c5145214dda2a183af97212878482aafbb2433d4f0ec0a2efeec7cc9ef2a
    INCLUDE $<SOURCE_DIR>/include
)

# VulkanSDK
Add3rdBinaryPackage(
    NAME VulkanSDK
    VERSION 1.3.243.0
    HASH
        Windows 23ded0e6155a8ea265da84bd62f7a45cc3fc637fb1eeba61a5b9430fa704e4b5
        Darwin a5c63a4f29984540efa00190240d31b59539413cf4783792bd9f53207f4f7d07
    INCLUDE
        Windows $<SOURCE_DIR>/Include
        Darwin $<SOURCE_DIR>/macOS/include
    LINK
        Windows $<SOURCE_DIR>/Lib
        Darwin $<SOURCE_DIR>/macOS/lib
    LIB
        Windows vulkan-1
        Darwin vulkan.1
)

# DXC
# Windows uses standalone package, macOS uses lib in VulkanSDK
Add3rdBinaryPackage(
    NAME dxc
    PLATFORM Windows
    VERSION 1.6.2112
    HASH b8d8e5cd03234e477bc3fe5d4258652a7fb61916141660408ee4c54a4f1eb939
    INCLUDE $<SOURCE_DIR>/inc
    LINK $<SOURCE_DIR>/lib/x64
    LIB dxcompiler
    RUNTIME_DEP
        Windows $<SOURCE_DIR>/bin/x64/dxcompiler.dll $<SOURCE_DIR>/bin/x64/dxil.dll
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
    ARCH
    NAME llvm-clang
    VERSION 15.0.7
    HASH
        Windows-x64 dd13894552677b689551248266b2ec4fc12341467776b282366098597b0c3501
        Darwin-x86_64 75b45dda76d52fcf7f3253aedea1f7327eb2aca37871b287d89234b55bde5504
        Darwin-arm64 99a4884945674163374d99c2220cadbfe20a947b1217193140c6b6638a3b0627
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB
        Windows-x64 libclang
        Darwin-x86_64 clang
        Darwin-arm64 clang
    RUNTIME_DEP
        Windows $<SOURCE_DIR>/bin/libclang.dll
)

# Google Test
Add3rdCMakeProject(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    HASH bb1146f051868868b96fa83069d4d05f0e05216b5c3f697859c591fd73b93a86
    CMAKE_ARG -Dgtest_force_shared_crt=ON
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB gtest$<IF:$<CONFIG:Debug>,d,> gmock$<IF:$<CONFIG:Debug>,d,>
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
    VERSION forked-2f248cfbf88d0895d069258e4971b5f2fc3a9f19
    HASH 5acf325794585578915dfa7fbac9eb2ca436ffe4831b05c40d1f286b68d7ac20
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB city
)

# stb-image
Add3rdHeaderOnlyPackage(
    NAME stb
    PLATFORM All
    VERSION master-8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55
    HASH 32e28f89a85b3bb6749e8caed9eb5226f7f5956769005d2b5a41d48d9481d90c
    INCLUDE $<SOURCE_DIR>
)

# fmt-lib
Add3rdHeaderOnlyPackage(
    NAME fmt-lib
    PLATFORM All
    VERSION 9.1.0
    HASH 09fb58ad10d1071ca8dbe0dc02b64fac6b2f4d4ba626b6d21a67d5b60ffe9f95
    INCLUDE $<SOURCE_DIR>/include
)

# spirv-cross
Add3rdCMakeProject(
    NAME spirv-cross
    PLATFORM All
    VERSION 1.3.243.0
    HASH 2b09e3cf9357156e8a4f1bd7cde3771184f652ec3b632993495748112a7f4665
    CMAKE_ARG -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_C_API=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB spirv-cross-core$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
        spirv-cross-msl$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
        spirv-cross-glsl$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
)

# assimp
Add3rdCMakeProject(
    NAME assimp-lib
    PLATFORM All
    VERSION 5.2.5
    HASH ae55d305a607ad8cc598c306663796dbf6e50a3cb0b952fe1ea3175f0f342ad9
    CMAKE_ARG -DASSIMP_BUILD_TESTS=OFF -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_INSTALL_PDB=OFF
    INCLUDE $<INSTALL_DIR>/include
    LINK $<INSTALL_DIR>/lib
    LIB
        Windows "assimp-vc${MSVC_TOOLSET_VERSION}-mt$<IF:$<CONFIG:Debug>,d,>"
        Darwin "assimp$<IF:$<CONFIG:Debug>,d,>"
    RUNTIME_DEP
        Windows "$<INSTALL_DIR>/bin/assimp-vc${MSVC_TOOLSET_VERSION}-mt$<IF:$<CONFIG:Debug>,d,>.dll"
)

# vma
Add3rdHeaderOnlyPackage(
    NAME VulkanMemoryAllocator
    PLATFORM All
    VERSION 3.0.1
    HASH a63a5e32c21fa9e839580d0c8727c6dead9da01730a8f0d10717cd784bc632de
    INCLUDE $<SOURCE_DIR>/include
)

# Qt6
Find3rdPackage(
    NAME Qt
    PACKAGE Qt6
    VERSION 6.5.2
    HASH
        Windows e2a573a53b6de88c0ce768868bd5a8258d432ad71c45a407afa55243b051668f
        Darwin 4379592e8b3af8eb32f5d030608651da8dc59c30b9bfa7837b7ed17949ff3365
    PREFIX
        Windows $<SOURCE_DIR>/6.5.2/msvc2019_64
        Darwin $<SOURCE_DIR>/6.5.2/macos
    COMPONENTS Core Gui Widgets
    LIB Qt::Core Qt::Gui Qt::Widgets
    RUNTIME_DEP
        Windows
            $<SOURCE_DIR>/6.5.2/msvc2019_64/plugins/platforms/qwindows$<IF:$<CONFIG:Debug>,d,>.dll
            $<SOURCE_DIR>/6.5.2/msvc2019_64/bin/Qt6Core$<IF:$<CONFIG:Debug>,d,>.dll
            $<SOURCE_DIR>/6.5.2/msvc2019_64/bin/Qt6Gui$<IF:$<CONFIG:Debug>,d,>.dll
            $<SOURCE_DIR>/6.5.2/msvc2019_64/bin/Qt6Widgets$<IF:$<CONFIG:Debug>,d,>.dll
)
if (${BUILD_EDITOR})
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif ()
