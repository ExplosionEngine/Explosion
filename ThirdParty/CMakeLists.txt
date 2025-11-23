set(3RD_REPO "http://1.13.181.171" CACHE STRING "" FORCE)
set(3RD_DIR ${CMAKE_SOURCE_DIR}/ThirdParty CACHE PATH "" FORCE)
set(3RD_ZIP_DIR ${3RD_DIR}/Zip CACHE PATH "" FORCE)
set(3RD_SOURCE_DIR ${3RD_DIR}/Lib CACHE PATH "" FORCE)
set(3RD_BINARY_DIR ${CMAKE_BINARY_DIR}/ThirdPartyBuild CACHE PATH "" FORCE)
set(3RD_INSTALL_DIR ${CMAKE_BINARY_DIR}/ThirdPartyInstall CACHE PATH "" FORCE)

function(download_and_setup_3rd_package)
    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs PLATFORM VERSION HASH)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    exp_get_3rd_platform_value(
        OUTPUT version_value
        INPUT ${arg_VERSION}
    )

    set(cur_platform All ${CMAKE_SYSTEM_NAME})
    foreach (platform ${cur_platform})
        if (${platform} IN_LIST arg_PLATFORM)
            set(full_name "${arg_NAME}-${platform}-${version_value}")
        endif ()
    endforeach ()

    if (NOT DEFINED full_name)
        set(${arg_NAME}_READY FALSE PARENT_SCOPE)
        return()
    endif ()

    set(source_dir ${3RD_SOURCE_DIR}/${full_name})
    exp_download_and_extract_3rd_package(
        URL ${3RD_REPO}/${full_name}.7z
        SAVE_AS ${3RD_ZIP_DIR}/${full_name}.7z
        EXTRACT_TO ${source_dir}
        HASH ${arg_HASH}
    )
    set(${arg_NAME}_READY TRUE PARENT_SCOPE)
    set(${arg_NAME}_SOURCE_DIR ${source_dir} PARENT_SCOPE)
    set(${arg_NAME}_BINARY_DIR ${3RD_BINARY_DIR}/${arg_NAME} PARENT_SCOPE)
    set(${arg_NAME}_INSTALL_DIR ${3RD_INSTALL_DIR}/${arg_NAME}/$<CONFIG> PARENT_SCOPE)
endfunction()

function(should_setup_3rd_package)
    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs PLATFORM)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    set(cur_platform All ${CMAKE_SYSTEM_NAME})
    foreach (platform ${cur_platform})
        if (${platform} IN_LIST arg_PLATFORM)
            set(${arg_NAME}_READY TRUE PARENT_SCOPE)
            return()
        endif ()
    endforeach ()

    set(${arg_NAME}_READY FALSE PARENT_SCOPE)
endfunction()

# DirectX 12 Headers
download_and_setup_3rd_package(
    NAME DirectXHeaders
    PLATFORM Windows
    VERSION 1.4.9
    HASH 0313c5145214dda2a183af97212878482aafbb2433d4f0ec0a2efeec7cc9ef2a
)
if (${DirectXHeaders_READY})
    exp_add_3rd_header_only_package(
        NAME DirectXHeaders
        SOURCE_DIR ${DirectXHeaders_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>/include
    )
endif ()

# VulkanSDK
set(VULKAN_SDK_VERSION 1.3.296.0)
download_and_setup_3rd_package(
    NAME VulkanSDK
    PLATFORM Windows Darwin
    VERSION ${VULKAN_SDK_VERSION}
    HASH
        Windows 27b8344a5b1333222d55c9c107914dba1cc9beb788b207e087c27dc7b8762816
        Darwin a792aaad3937a694a4cb853d2fbb3fa7025ba3754bedd52799e687a4c254129c
)
if (${VulkanSDK_READY})
    exp_add_3rd_binary_package(
        NAME VulkanSDK
        SOURCE_DIR ${VulkanSDK_SOURCE_DIR}
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
endif ()

# DXC
# Windows uses standalone package, macOS uses lib in VulkanSDK
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    download_and_setup_3rd_package(
        NAME dxc
        PLATFORM Windows
        VERSION 1.6.2112
        HASH b8d8e5cd03234e477bc3fe5d4258652a7fb61916141660408ee4c54a4f1eb939
    )
    if (${dxc_READY})
        exp_add_3rd_binary_package(
            NAME dxc
            SOURCE_DIR ${dxc_SOURCE_DIR}
            INCLUDE $<SOURCE_DIR>/inc
            LINK $<SOURCE_DIR>/lib/x64
            LIB dxcompiler
            RUNTIME_DEP
                Windows $<SOURCE_DIR>/bin/x64/dxcompiler.dll $<SOURCE_DIR>/bin/x64/dxil.dll
        )
    endif ()
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    should_setup_3rd_package(
        NAME dxc
        PLATFORM Darwin
    )
    if (${dxc_READY})
        exp_add_3rd_alias_package(
            NAME dxc
            LIB dxcompiler
        )
    endif ()
endif ()

# GLFW
download_and_setup_3rd_package(
    NAME glfw
    PLATFORM All
    VERSION 4.0
    HASH 2483836f085d616acaa48f6dc8efec5239f6a538c3a25d13fdc6a7afb2cce1d4
)
if (${glfw_READY})
    exp_add_3rd_cmake_package(
        NAME glfw
        SOURCE_DIR ${glfw_SOURCE_DIR}
        BINARY_DIR ${glfw_BINARY_DIR}
        INSTALL_DIR ${glfw_INSTALL_DIR}
        CMAKE_ARG -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
        INCLUDE $<INSTALL_DIR>/include
        LINK $<INSTALL_DIR>/lib
        LIB glfw3
    )
endif ()

# clipp
download_and_setup_3rd_package(
    NAME clipp
    PLATFORM All
    VERSION forked-f7cffbd61a7b64189d6573e8c3848d094c35168a
    HASH 59db8b8586381652b5e0ae629a9d32e0e443428bc4d94af091b9525a62d167fb
)
if (${clipp_READY})
    exp_add_3rd_header_only_package(
        NAME clipp
        SOURCE_DIR ${clipp_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>/include
    )
endif ()

# DebugBreak
download_and_setup_3rd_package(
    NAME debugbreak
    PLATFORM All
    VERSION 1.0
    HASH 16e35a330a9927181ed2e03a92efe6d39670b33c1bdf61ab88d29673ef3a89d4
)
if (${debugbreak_READY})
    exp_add_3rd_header_only_package(
        NAME debugbreak
        SOURCE_DIR ${debugbreak_SOURCE_DIR}
        INSTALL_DIR ${debugbreak_INSTALL_DIR}
        INSTALL_FILES debugbreak.h
        INCLUDE $<INSTALL_DIR>
    )
endif ()

# LLVM-Clang
download_and_setup_3rd_package(
    NAME llvm-clang
    PLATFORM Windows Darwin
    VERSION 19.1.6
    HASH
        Windows f68b922c87a99b22c0b53a5cbd55db86bd3870c16c4e9060efaecfb9ce49e726
        Darwin 08fdf1b6208823acadc23027fda1f12e5171101ec05fa526b545088be31fe19d
)
if (${llvm-clang_READY})
    exp_add_3rd_binary_package(
        NAME llvm-clang
        SOURCE_DIR ${llvm-clang_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>/include
        LINK $<SOURCE_DIR>/lib
        LIB
            Windows libclang
            Darwin clang
        RUNTIME_DEP
            Windows $<SOURCE_DIR>/bin/libclang.dll
            Darwin $<SOURCE_DIR>/lib/libclang.dylib
    )
endif ()

# Google Test
download_and_setup_3rd_package(
    NAME googletest
    PLATFORM All
    VERSION 1.11.0
    HASH bb1146f051868868b96fa83069d4d05f0e05216b5c3f697859c591fd73b93a86
)
if (${googletest_READY})
    exp_add_3rd_cmake_package(
        NAME googletest
        SOURCE_DIR ${googletest_SOURCE_DIR}
        BINARY_DIR ${googletest_BINARY_DIR}
        INSTALL_DIR ${googletest_INSTALL_DIR}
        CMAKE_ARG -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -Dgtest_force_shared_crt=ON
        INCLUDE $<INSTALL_DIR>/include
        LINK $<INSTALL_DIR>/lib
        LIB gtest$<IF:$<CONFIG:Debug>,d,> gmock$<IF:$<CONFIG:Debug>,d,>
    )
endif ()

# TaskFlow
download_and_setup_3rd_package(
    NAME taskflow
    PLATFORM All
    VERSION 3.4.0
    HASH 774e23d5b6be17a5482d70ffd76ba4ccfd8821c86b7c3f0bcdcd5d96bc264041
)
if (${taskflow_READY})
    exp_add_3rd_header_only_package(
        NAME taskflow
        SOURCE_DIR ${taskflow_SOURCE_DIR}
        INSTALL_DIR ${taskflow_INSTALL_DIR}
        INSTALL_FILES taskflow
        INCLUDE $<INSTALL_DIR>
    )
endif ()

# CityHash
download_and_setup_3rd_package(
    NAME cityhash
    PLATFORM All
    VERSION forked-2f248cfbf88d0895d069258e4971b5f2fc3a9f19
    HASH 5acf325794585578915dfa7fbac9eb2ca436ffe4831b05c40d1f286b68d7ac20
)
if (${cityhash_READY})
    exp_add_3rd_cmake_package(
        NAME cityhash
        SOURCE_DIR ${cityhash_SOURCE_DIR}
        BINARY_DIR ${cityhash_BINARY_DIR}
        INSTALL_DIR ${cityhash_INSTALL_DIR}
        INCLUDE $<INSTALL_DIR>/include
        LINK $<INSTALL_DIR>/lib
        LIB city
    )
endif ()

# stb-image
download_and_setup_3rd_package(
    NAME stb
    PLATFORM All
    VERSION master-8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55
    HASH 32e28f89a85b3bb6749e8caed9eb5226f7f5956769005d2b5a41d48d9481d90c
)
if (${stb_READY})
    exp_add_3rd_header_only_package(
        NAME stb
        SOURCE_DIR ${stb_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>
    )
endif ()

# spirv-cross
download_and_setup_3rd_package(
    NAME spirv-cross
    PLATFORM All
    VERSION 1.3.243.0
    HASH 2b09e3cf9357156e8a4f1bd7cde3771184f652ec3b632993495748112a7f4665
)
if (${spirv-cross_READY})
    exp_add_3rd_cmake_package(
        NAME spirv-cross
        SOURCE_DIR ${spirv-cross_SOURCE_DIR}
        BINARY_DIR ${spirv-cross_BINARY_DIR}
        INSTALL_DIR ${spirv-cross_INSTALL_DIR}
        CMAKE_ARG -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DSPIRV_CROSS_CLI=OFF -DSPIRV_CROSS_ENABLE_C_API=OFF -DSPIRV_CROSS_ENABLE_TESTS=OFF
        INCLUDE $<INSTALL_DIR>/include
        LINK $<INSTALL_DIR>/lib
        LIB spirv-cross-core$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
            spirv-cross-msl$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
            spirv-cross-glsl$<IF:$<AND:$<CONFIG:Debug>,$<PLATFORM_ID:Windows>>,d,>
    )
endif ()

# assimp
download_and_setup_3rd_package(
    NAME assimp-lib
    PLATFORM All
    VERSION 5.2.5
    HASH ae55d305a607ad8cc598c306663796dbf6e50a3cb0b952fe1ea3175f0f342ad9
)
if (${assimp-lib_READY})
    exp_add_3rd_cmake_package(
        NAME assimp-lib
        SOURCE_DIR ${assimp-lib_SOURCE_DIR}
        BINARY_DIR ${assimp-lib_BINARY_DIR}
        INSTALL_DIR ${assimp-lib_INSTALL_DIR}
        CMAKE_ARG -DASSIMP_BUILD_TESTS=OFF -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_INSTALL_PDB=OFF
        INCLUDE $<INSTALL_DIR>/include
        LINK $<INSTALL_DIR>/lib
        LIB
            Windows "assimp-vc${MSVC_TOOLSET_VERSION}-mt$<IF:$<CONFIG:Debug>,d,>"
            Darwin "assimp$<IF:$<CONFIG:Debug>,d,>"
        RUNTIME_DEP
            Windows "$<INSTALL_DIR>/bin/assimp-vc${MSVC_TOOLSET_VERSION}-mt$<IF:$<CONFIG:Debug>,d,>.dll"
    )
endif ()

# vma
download_and_setup_3rd_package(
    NAME VulkanMemoryAllocator
    PLATFORM All
    VERSION 3.0.1
    HASH a63a5e32c21fa9e839580d0c8727c6dead9da01730a8f0d10717cd784bc632de
)
if (${VulkanMemoryAllocator_READY})
    exp_add_3rd_header_only_package(
        NAME VulkanMemoryAllocator
        SOURCE_DIR ${VulkanMemoryAllocator_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>/include
    )
endif ()

# Qt6
set(QT_VERSION "6.9.1" CACHE STRING "" FORCE)
download_and_setup_3rd_package(
    NAME Qt
    PLATFORM Windows Darwin
    VERSION ${QT_VERSION}
    HASH
        Windows 9cd849f79784fda5436b0da3b7625b4530a77b93c5e80ea9a2d8a95060c174b6
        Darwin 3938c6156228fcf63c33029067bd4b6dd333ed284e069198b85d92de3d9b3e13
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(QT_LIB_PREFIX ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2022_64 CACHE PATH "" FORCE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(QT_LIB_PREFIX ${Qt_SOURCE_DIR}/${QT_VERSION}/macos CACHE PATH "" FORCE)

    # NOTICE: Fix Qt 6.9.1 build on macOS 26, #see https://codereview.qt-project.org/c/qt/qtbase/+/652022/3/cmake/FindWrapOpenGL.cmake#b50
    if (QT_VERSION STREQUAL "6.9.1")
        file(COPY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Patch/Qt-Darwin-6.9.1-FindWrapOpenGL.cmake ${QT_LIB_PREFIX}/lib/cmake/Qt6/FindWrapOpenGL.cmake)
    endif ()
endif ()

# rapidjson
download_and_setup_3rd_package(
    NAME rapidjson
    PLATFORM All
    VERSION d621dc9
    HASH 696f6ca1ecca9d13170c0a47eda66a3015bcf02a7b4bdd23f574ea302eb4bf3e
)
if (${rapidjson_READY})
    exp_add_3rd_header_only_package(
        NAME rapidjson
        SOURCE_DIR ${rapidjson_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>/include
    )
endif ()

# cpp-httplib
download_and_setup_3rd_package(
    NAME cpp-httplib
    PLATFORM All
    VERSION 0.24.0
    HASH a0320b106f4a8f945ec2556e62fffab05b068f2ddfff99c6a367b2e16c14bde1
)
if (${cpp-httplib_READY})
    exp_add_3rd_header_only_package(
        NAME cpp-httplib
        SOURCE_DIR ${cpp-httplib_SOURCE_DIR}
        INCLUDE $<SOURCE_DIR>
    )
endif ()
