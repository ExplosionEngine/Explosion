# DirectX 12 Headers
Add3rdHeaderOnlyPackage(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    HASH 0313c5145214dda2a183af97212878482aafbb2433d4f0ec0a2efeec7cc9ef2a
    INCLUDE $<SOURCE_DIR>/include
)

# VulkanSDK
set(VULKAN_SDK_VERSION 1.3.296.0)
Add3rdBinaryPackage(
    NAME VulkanSDK
    VERSION ${VULKAN_SDK_VERSION}
    HASH
        Windows 27b8344a5b1333222d55c9c107914dba1cc9beb788b207e087c27dc7b8762816
        Darwin a792aaad3937a694a4cb853d2fbb3fa7025ba3754bedd52799e687a4c254129c
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
if (NOT ${CI})
    #see https://github.com/KhronosGroup/Vulkan-Loader/blob/main/docs/LoaderLayerInterface.md
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        execute_process(COMMAND reg query "HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers" OUTPUT_VARIABLE LAYERS)
        string(REGEX MATCH "VkLayer_khronos_validation\.json" MATCH_RESULT ${LAYERS})
        list(LENGTH MATCH_RESULT MATCH_RESULT_LENGTH)

        if (${MATCH_RESULT_LENGTH} GREATER 0)
            message("Used registered validation layer in HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
        else ()
            set(VALIDATION_LAYER_JSON ${3RD_SOURCE_DIR}/VulkanSDK-Windows-${VULKAN_SDK_VERSION}/Bin/VkLayer_khronos_validation.json)
            message("Found no registered validation layer, will register ${VALIDATION_LAYER_JSON} to reg HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
            string(REPLACE "/" "\\" VALIDATION_LAYER_JSON ${VALIDATION_LAYER_JSON})
            execute_process(COMMAND PowerShell -Command "Start-Process reg -ArgumentList 'add HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers /v ${VALIDATION_LAYER_JSON} /t REG_DWORD /d 0' -Verb RunAs")
        endif ()
    else (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        # new version of vulkaninfo application use volk to perform dynamic loading, which need libvulkan.dylib in working directory or system,
        # so if vulkaninfo print initialized failed, we need copy vulkan dynamic lib to working directory
        set(PLATFORM_VULKAN_SDK_DIR ${CMAKE_SOURCE_DIR}/ThirdParty/Lib/VulkanSDK-Darwin-${VULKAN_SDK_VERSION}/macOS)
        execute_process(COMMAND ${PLATFORM_VULKAN_SDK_DIR}/bin/vulkaninfo WORKING_DIRECTORY ${PLATFORM_VULKAN_SDK_DIR}/bin OUTPUT_VARIABLE VULKAN_INFO ERROR_VARIABLE VULKAN_INFO)
        string(REGEX MATCH "Failed to initialize" MATCH_RESULT ${VULKAN_INFO})
        list(LENGTH MATCH_RESULT MATCH_RESULT_LENGTH)
        if (${MATCH_RESULT_LENGTH} GREATER 0)
            set(SRC_FILE ${PLATFORM_VULKAN_SDK_DIR}/lib/libvulkan.dylib)
            set(DST_FILE ${PLATFORM_VULKAN_SDK_DIR}/bin/libvulkan.dylib)
            message("vulkaninfo initialized failed, perform vulkan dynamic library copy: ${SRC_FILE} -> ${DST_FILE}")
            file(COPY_FILE ${SRC_FILE} ${DST_FILE} ONLY_IF_DIFFERENT)
        else ()
            message("vulkaninfo test success")
        endif ()

        # check drivers
        execute_process(COMMAND ${PLATFORM_VULKAN_SDK_DIR}/bin/vulkaninfo WORKING_DIRECTORY ${PLATFORM_VULKAN_SDK_DIR}/bin OUTPUT_VARIABLE VULKAN_INFO ERROR_VARIABLE VULKAN_INFO)
        string(REGEX MATCH "ERROR_INCOMPATIBLE_DRIVER" MATCH_RESULT ${VULKAN_INFO})
        list(LENGTH MATCH_RESULT MATCH_RESULT_LENGTH)
        if (${MATCH_RESULT_LENGTH} GREATER 0)
            message("there is no drivers found for vulkan, copy and rewrite driver files to ~/.config/vulkan/icd.d")
            get_filename_component(DRIVERS_DIR_ABSOLUTE ~/.config/vulkan/icd.d ABSOLUTE)
            if (NOT EXISTS ${DRIVERS_DIR_ABSOLUTE})
                file(MAKE_DIRECTORY ${DRIVERS_DIR_ABSOLUTE})
            endif ()

            file(GLOB DRIVER_FILES ${PLATFORM_VULKAN_SDK_DIR}/share/vulkan/icd.d/*.json)
            foreach (DRIVER_FILE ${DRIVER_FILES})
                file(READ ${DRIVER_FILE} FILE_CONTENT)
                string(REPLACE "../../../lib" "${PLATFORM_VULKAN_SDK_DIR}/lib" FILE_CONTENT ${FILE_CONTENT})
                get_filename_component(FILE_NAME ${DRIVER_FILE} NAME)
                file(WRITE ${DRIVERS_DIR_ABSOLUTE}/${FILE_NAME} ${FILE_CONTENT})
            endforeach ()
        else ()
            message("found vulkan drivers installed in system, will use it")
        endif ()

        # check layers
        execute_process(COMMAND ${PLATFORM_VULKAN_SDK_DIR}/bin/vulkaninfo WORKING_DIRECTORY ${PLATFORM_VULKAN_SDK_DIR}/bin OUTPUT_VARIABLE VULKAN_INFO ERROR_VARIABLE VULKAN_INFO)
        string(REGEX MATCH "Layers:\n=======" MATCH_RESULT ${VULKAN_INFO})
        list(LENGTH MATCH_RESULT MATCH_RESULT_LENGTH)
        if (${MATCH_RESULT_LENGTH} GREATER 0)
            message("there is no layers found for vulkan, copy and rewrite layer files to ~/.config/vulkan/explicit_layer.d")
            get_filename_component(LAYERS_DIR_ABSOLUTE ~/.config/vulkan/explicit_layer.d ABSOLUTE)
            if (NOT EXISTS ${LAYERS_DIR_ABSOLUTE})
                file(MAKE_DIRECTORY ${LAYERS_DIR_ABSOLUTE})
            endif ()

            file(GLOB LAYER_FILES ${PLATFORM_VULKAN_SDK_DIR}/share/vulkan/explicit_layer.d/*.json)
            foreach (LAYER_FILE ${LAYER_FILES})
                file(READ ${LAYER_FILE} FILE_CONTENT)
                string(REPLACE "../../../lib" "${PLATFORM_VULKAN_SDK_DIR}/lib" FILE_CONTENT ${FILE_CONTENT})
                get_filename_component(FILE_NAME ${LAYER_FILE} NAME)
                file(WRITE ${LAYERS_DIR_ABSOLUTE}/${FILE_NAME} ${FILE_CONTENT})
            endforeach ()
        else ()
            message("found vulkan layers installed in system, will use it")
        endif ()
    endif ()
endif ()

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
    VERSION 4.0
    HASH 2483836f085d616acaa48f6dc8efec5239f6a538c3a25d13fdc6a7afb2cce1d4
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
    ARCH
    VERSION
        Windows-AMD64 19.1.6
        Darwin-arm64 19.1.6
    HASH
        Windows-AMD64 f68b922c87a99b22c0b53a5cbd55db86bd3870c16c4e9060efaecfb9ce49e726
        Darwin-arm64 08fdf1b6208823acadc23027fda1f12e5171101ec05fa526b545088be31fe19d
    INCLUDE $<SOURCE_DIR>/include
    LINK $<SOURCE_DIR>/lib
    LIB
        Windows-AMD64 libclang
        Darwin-arm64 clang
    RUNTIME_DEP
        Windows $<SOURCE_DIR>/bin/libclang.dll
        Darwin $<SOURCE_DIR>/lib/libclang.dylib
)

# Google Test
Add3rdCMakeProject(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    HASH bb1146f051868868b96fa83069d4d05f0e05216b5c3f697859c591fd73b93a86
    CMAKE_ARG -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Dgtest_force_shared_crt=ON
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

# spirv-cross
Add3rdCMakeProject(
    NAME spirv-cross
    PLATFORM All
    VERSION 1.3.243.0
    HASH 2b09e3cf9357156e8a4f1bd7cde3771184f652ec3b632993495748112a7f4665
    CMAKE_ARG -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_C_API=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
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
set(QT_VERSION "6.9.1" CACHE STRING "" FORCE)
Setup3rdPackage(
    NAME Qt
    VERSION ${QT_VERSION}
    HASH
        Windows 9cd849f79784fda5436b0da3b7625b4530a77b93c5e80ea9a2d8a95060c174b6
        Darwin 3938c6156228fcf63c33029067bd4b6dd333ed284e069198b85d92de3d9b3e13
    PREFIX
        Windows $<SOURCE_DIR>/6.9.1/msvc2019_64
        Darwin $<SOURCE_DIR>/6.9.1/macos
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(QT_LIB_PREFIX ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2022_64 CACHE PATH "" FORCE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(QT_LIB_PREFIX ${Qt_SOURCE_DIR}/${QT_VERSION}/macos CACHE PATH "" FORCE)
endif ()

# rapidjson
Add3rdHeaderOnlyPackage(
    NAME rapidjson
    PLATFORM All
    VERSION d621dc9
    HASH 696f6ca1ecca9d13170c0a47eda66a3015bcf02a7b4bdd23f574ea302eb4bf3e
    INCLUDE $<SOURCE_DIR>/include
)

# Node.js
set(NODEJS_VERSION "22.18.0" CACHE STRING "" FORCE)
Add3rdBinaryPackage(
    NAME Nodejs
    VERSION ${NODEJS_VERSION}
    HASH
        Windows 6c5fc993dacd58f2e3b145b06599e8a3aa1b0d0c2dea48b0a09af31c5c6ebe23
        Darwin 0f6e16a5564c29aa45d2354ac66ea5f66232a7c45baa45fec372edb00e491c16
)

# cpp-httplib
set(CPP_HTTP_LIB_VERSION "0.24.0")
Add3rdHeaderOnlyPackage(
    NAME cpp-httplib
    PLATFORM All
    VERSION 0.24.0
    HASH a0320b106f4a8f945ec2556e62fffab05b068f2ddfff99c6a367b2e16c14bde1
    INCLUDE $<SOURCE_DIR>
)
set(CPP_HTTP_LIB_SOURCE_DIR ${3RD_SOURCE_DIR}/cpp-httplib-All-${CPP_HTTP_LIB_VERSION} CACHE STRING "" FORCE)
