# EnTT
AddThirdPartyPackage(
    NAME EnTT
    VERSION 3.8.1
)
set(EnTT_INC ${EnTT_SOURCE_DIR}/single_include CACHE PATH "" FORCE)

# GLM
AddThirdPartyPackage(
    NAME GLM
    VERSION 0.9.9.8
)
set(GLM_INC ${GLM_SOURCE_DIR} CACHE PATH "" FORCE)

# Meta
AddThirdPartyPackage(
    NAME Meta
    VERSION 1.4.9
)
set(Meta_INC ${Meta_SOURCE_DIR}/src CACHE PATH "" FORCE)

# DirectX 12 Headers
AddThirdPartyPackage(
    NAME DirectXHeaders
    VERSION 1.4.9
)
set(DirectXHeaders_INC ${DirectXHeaders_SOURCE_DIR}/include CACHE PATH "" FORCE)

# VulkanSDK
AddThirdPartyPackage(
    PLATFORM
    NAME VulkanSDK
    VERSION 1.2.198.1
)
set(VulkanSDK_INC ${VulkanSDK_SOURCE_DIR}/Include CACHE PATH "" FORCE)
set(VulkanSDK_LINK ${VulkanSDK_SOURCE_DIR}/Lib CACHE PATH "" FORCE)
set(VulkanSDK_BIN ${VulkanSDK_SOURCE_DIR}/Bin CACHE PATH "" FORCE)
set(VulkanSDK_VULKAN_LIB vulkan-1 CACHE PATH "" FORCE)

# GLFW
AddThirdPartyPackage(
    NAME glfw
    VERSION 3.3.6
    BUILD
    ARG -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF
)
set(glfw_INC ${glfw_INSTALL_DIR}/include CACHE PATH "" FORCE)
set(glfw_LINK ${glfw_INSTALL_DIR}/lib CACHE PATH "" FORCE)
set(glfw_LIB glfw3 CACHE STRING "" FORCE)
