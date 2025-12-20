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

# conan managed
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_library(OpenGLDummy INTERFACE)
    add_library(opengl::opengl ALIAS OpenGLDummy)
endif ()

find_package(httplib REQUIRED GLOBAL)
find_package(glfw3 REQUIRED GLOBAL)
find_package(stb REQUIRED GLOBAL)
find_package(cityhash REQUIRED GLOBAL)
find_package(GTest REQUIRED GLOBAL)
find_package(Taskflow REQUIRED GLOBAL)
find_package(libclang REQUIRED GLOBAL)
find_package(assimp REQUIRED GLOBAL)
find_package(VulkanMemoryAllocator REQUIRED GLOBAL)
find_package(debugbreak REQUIRED GLOBAL)
find_package(rapidjson REQUIRED GLOBAL)
find_package(clipp REQUIRED GLOBAL)
find_package(dxc REQUIRED GLOBAL)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    find_package(DirectX-Headers REQUIRED GLOBAL)
    set_target_properties(assimp::assimp PROPERTIES RUNTIME_DEP "${assimp_INCLUDE_DIR}/../bin/assimp-vc${MSVC_TOOLSET_VERSION}-mt.dll")
    set_target_properties(libclang::libclang PROPERTIES RUNTIME_DEP "${libclang_INCLUDE_DIR}/../bin/libclang.dll")
    set_target_properties(dxc::dxc PROPERTIES RUNTIME_DEP "${dxc_INCLUDE_DIR}/../bin/dxil.dll;${dxc_INCLUDE_DIR}/../bin/")
endif ()
