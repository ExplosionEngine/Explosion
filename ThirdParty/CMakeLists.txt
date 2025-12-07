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
            execute_process(COMMAND reg query "HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers" OUTPUT_VARIABLE layers)
            string(REGEX MATCH "VkLayer_khronos_validation\.json" match_result ${layers})
            list(LENGTH match_result match_result_length)

            if (${match_result_length} GREATER 0)
                message("Used registered validation layer in HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
            else ()
                set(validation_layer_json ${3RD_SOURCE_DIR}/VulkanSDK-Windows-${VULKAN_SDK_VERSION}/Bin/VkLayer_khronos_validation.json)
                message("Found no registered validation layer, will register ${validation_layer_json} to reg HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers")
                string(REPLACE "/" "\\" validation_layer_json ${validation_layer_json})
                execute_process(COMMAND PowerShell -Command "Start-Process reg -ArgumentList 'add HKEY_LOCAL_MACHINE\\SOFTWARE\\Khronos\\Vulkan\\ExplicitLayers /v ${validation_layer_json} /t REG_DWORD /d 0' -Verb RunAs")
            endif ()
        else (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            # new version of vulkaninfo application use volk to perform dynamic loading, which need libvulkan.dylib in working directory or system,
            # so if vulkaninfo print initialized failed, we need copy vulkan dynamic lib to working directory
            set(platform_vulkan_sdk_dir ${CMAKE_SOURCE_DIR}/ThirdParty/Lib/VulkanSDK-Darwin-${VULKAN_SDK_VERSION}/macOS)
            execute_process(COMMAND ${platform_vulkan_sdk_dir}/bin/vulkaninfo WORKING_DIRECTORY ${platform_vulkan_sdk_dir}/bin OUTPUT_VARIABLE vulkan_info ERROR_VARIABLE vulkan_info)
            string(REGEX MATCH "Failed to initialize" match_result ${vulkan_info})
            list(LENGTH match_result match_result_length)
            if (${match_result_length} GREATER 0)
                set(src_file ${platform_vulkan_sdk_dir}/lib/libvulkan.dylib)
                set(dst_file ${platform_vulkan_sdk_dir}/bin/libvulkan.dylib)
                message("vulkaninfo initialized failed, perform vulkan dynamic library copy: ${src_file} -> ${dst_file}")
                file(COPY_FILE ${src_file} ${dst_file} ONLY_IF_DIFFERENT)
            else ()
                message("vulkaninfo test success")
            endif ()

            # check drivers
            execute_process(COMMAND ${platform_vulkan_sdk_dir}/bin/vulkaninfo WORKING_DIRECTORY ${platform_vulkan_sdk_dir}/bin OUTPUT_VARIABLE vulkan_info ERROR_VARIABLE vulkan_info)
            string(REGEX MATCH "ERROR_INCOMPATIBLE_DRIVER" match_result ${vulkan_info})
            list(LENGTH match_result match_result_length)
            if (${match_result_length} GREATER 0)
                message("there is no drivers found for vulkan, copy and rewrite driver files to ~/.config/vulkan/icd.d")
                get_filename_component(drivers_dir_absolute ~/.config/vulkan/icd.d ABSOLUTE)
                if (NOT EXISTS ${drivers_dir_absolute})
                    file(MAKE_DIRECTORY ${drivers_dir_absolute})
                endif ()

                file(GLOB driver_files ${platform_vulkan_sdk_dir}/share/vulkan/icd.d/*.json)
                foreach (driver_file ${driver_files})
                    file(READ ${driver_file} file_content)
                    string(REPLACE "../../../lib" "${platform_vulkan_sdk_dir}/lib" file_content ${file_content})
                    get_filename_component(file_name ${driver_file} NAME)
                    file(WRITE ${drivers_dir_absolute}/${file_name} ${file_content})
                endforeach ()
            else ()
                message("found vulkan drivers installed in system, will use it")
            endif ()

            # check layers
            execute_process(COMMAND ${platform_vulkan_sdk_dir}/bin/vulkaninfo WORKING_DIRECTORY ${platform_vulkan_sdk_dir}/bin OUTPUT_VARIABLE vulkan_info ERROR_VARIABLE vulkan_info)
            string(REGEX MATCH "Layers:\n=======" match_result ${vulkan_info})
            list(LENGTH match_result match_result_length)
            if (${match_result_length} GREATER 0)
                message("there is no layers found for vulkan, copy and rewrite layer files to ~/.config/vulkan/explicit_layer.d")
                get_filename_component(layers_dir_absolute ~/.config/vulkan/explicit_layer.d ABSOLUTE)
                if (NOT EXISTS ${layers_dir_absolute})
                    file(MAKE_DIRECTORY ${layers_dir_absolute})
                endif ()

                file(GLOB layer_files ${platform_vulkan_sdk_dir}/share/vulkan/explicit_layer.d/*.json)
                foreach (layer_file ${layer_files})
                    file(READ ${layer_file} file_content)
                    string(REPLACE "../../../lib" "${platform_vulkan_sdk_dir}/lib" file_content ${file_content})
                    get_filename_component(file_name ${layer_file} NAME)
                    file(WRITE ${layers_dir_absolute}/${file_name} ${file_content})
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
        LIB spirv-cross-core
            spirv-cross-msl
            spirv-cross-glsl
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

# conan managed
find_package(httplib REQUIRED GLOBAL)
find_package(glfw3 REQUIRED GLOBAL)
find_package(stb REQUIRED GLOBAL)
find_package(cityhash REQUIRED GLOBAL)
find_package(GTest REQUIRED GLOBAL)
find_package(Taskflow REQUIRED GLOBAL)
find_package(libclang REQUIRED GLOBAL)
find_package(assimp REQUIRED GLOBAL)

set(QT_VERSION "6.8.3" CACHE STRING "" FORCE)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    find_package(DirectX-Headers REQUIRED GLOBAL)
    set_target_properties(assimp::assimp PROPERTIES RUNTIME_DEP "${assimp_INCLUDE_DIR}/../bin/assimp-vc${MSVC_TOOLSET_VERSION}-mt.dll")
    set_target_properties(libclang::libclang PROPERTIES RUNTIME_DEP "${libclang_INCLUDE_DIR}/../bin/libclang.dll")
endif ()
