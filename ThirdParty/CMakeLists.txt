# EnTT
AddThirdPartyPackage(
    NAME EnTT
    VERSION 3.8.1
    HASH 77e55bfc4774c1be3a5a6a7a35b321a20c5dff37e688af3895e20cc56011e5f4
)
set(EnTT_INC ${EnTT_SOURCE_DIR}/single_include CACHE PATH "" FORCE)

# GLM
AddThirdPartyPackage(
    NAME GLM
    VERSION 0.9.9.8
    HASH 29876d3440386464c781853caf73eb49eaa1b04a5e648a0cff92381c5e9e69b4
)
set(GLM_INC ${GLM_SOURCE_DIR} CACHE PATH "" FORCE)

# Meta
AddThirdPartyPackage(
    NAME Meta
    VERSION 1.4.9
    HASH 9cd04221c4865fdef37f8a92646087ec777ded6b1ff1a5e932cd0cd72b175eba
)
set(Meta_INC ${Meta_SOURCE_DIR}/src CACHE PATH "" FORCE)

# DirectX 12 Headers
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    AddThirdPartyPackage(
        NAME DirectXHeaders
        VERSION 1.4.9
        HASH 32a28d1172eb5d6b1574a2ba51d628ba446f0e8a1d3291b5a4903a661c430738
    )
    set(DirectXHeaders_INC ${DirectXHeaders_SOURCE_DIR}/include CACHE PATH "" FORCE)
endif()

# VulkanSDK
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    AddThirdPartyPackage(
        NAME VulkanSDK-Windows
        HASH 006dfa3a63644fed382089402f64095ee9ca8ce3ec3107b80aae6e8941de9e9b
        VERSION 1.2.198.1
    )
    set(VulkanSDK_INC ${VulkanSDK-Windows_SOURCE_DIR}/Include CACHE PATH "" FORCE)
    set(VulkanSDK_LINK ${VulkanSDK-Windows_SOURCE_DIR}/Lib CACHE PATH "" FORCE)
    set(VulkanSDK_BIN ${VulkanSDK-Windows_SOURCE_DIR}/Bin CACHE PATH "" FORCE)
    set(VulkanSDK_VULKAN_LIB vulkan-1 CACHE PATH "" FORCE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    AddThirdPartyPackage(
        NAME VulkanSDK-Darwin
        HASH 6394c8e1f355820a8f6808bab2d87d518ee284e42b761da24e516f8ab0e2f75d
        VERSION 1.2.198.1
    )
    set(VulkanSDK_INC ${VulkanSDK-Darwin_SOURCE_DIR}/macOS/include CACHE PATH "" FORCE)
    set(VulkanSDK_LINK ${VulkanSDK-Darwin_SOURCE_DIR}/macOS/lib CACHE PATH "" FORCE)
    set(VulkanSDK_BIN ${VulkanSDK-Darwin_SOURCE_DIR}/macOS/bin CACHE PATH "" FORCE)
    set(VulkanSDK_VULKAN_LIB vulkan.1 CACHE STRING "" FORCE)
endif()

# ShaderConductor
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    AddThirdPartyPackage(
        NAME ShaderConductor-Windows
        HASH d31bf92bb922dda070fac67affad3ae728ef86b29c27e8c775b051336293f468
        VERSION 20200430.1
    )
    set(ShaderConductor_BIN ${ShaderConductor-Windows_SOURCE_DIR}/Bin CACHE PATH "" FORCE)
    set(ShaderConductor_INC ${ShaderConductor-Windows_SOURCE_DIR}/Include CACHE PATH "" FORCE)
    set(ShaderConductor_LINK ${ShaderConductor-Windows_SOURCE_DIR}/Lib CACHE PATH "" FORCE)
    set(ShaderConductor_LIB ShaderConductor CACHE STRING "" FORCE)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    AddThirdPartyPackage(
        NAME ShaderConductor-Darwin
        HASH 9701c4f58014d1b087e2b0f2d22f01d35b505078813579050a4e3ccab1124275
        VERSION 20200430.1
    )
    set(ShaderConductor_BIN ${ShaderConductor-Darwin_SOURCE_DIR}/Bin CACHE PATH "" FORCE)
    set(ShaderConductor_INC ${ShaderConductor-Darwin_SOURCE_DIR}/Include CACHE PATH "" FORCE)
    set(ShaderConductor_LINK ${ShaderConductor-Darwin_SOURCE_DIR}/Lib CACHE PATH "" FORCE)
    set(ShaderConductor_LIB "dxcompiler ShaderConductor" CACHE STRING "" FORCE)
endif()

# GLFW
AddThirdPartyPackage(
    NAME glfw
    VERSION 3.3.6
    BUILD
    HASH e2eb069a70da0fc862af4d14e6a80748883f12e20bdce101c028968165d9c688
    ARG -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF
)
set(glfw_INC ${glfw_INSTALL_DIR}/include CACHE PATH "" FORCE)
set(glfw_LINK ${glfw_INSTALL_DIR}/lib CACHE PATH "" FORCE)
set(glfw_LIB glfw3 CACHE STRING "" FORCE)

# clipp
AddThirdPartyPackage(
    NAME clipp
    VERSION 1.2.3
    HASH 90ff9435eb7610cb365f3e891d8d30c8ed864b3ac221dd1d02480e831016c64a
)
set(clipp_INC ${clipp_SOURCE_DIR}/include CACHE PATH "" FORCE)
