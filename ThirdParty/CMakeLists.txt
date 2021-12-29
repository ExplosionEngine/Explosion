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

# VulkanHpp
AddThirdPartyPackage(
    NAME VulkanHpp
    VERSION 1.2.202
)
set(VulkanHpp_INC ${VulkanHpp_SOURCE_DIR} CACHE PATH "" FORCE)

# GLFW
AddThirdPartyPackage(
    NAME glfw
    VERSION 3.3.6
    BUILD
)
set(glfw_INC ${glfw_INSTALL_DIR}/include CACHE PATH "" FORCE)
set(glfw_LINK ${glfw_INSTALL_DIR}/lib CACHE PATH "" FORCE)
set(glfw_LIB glfw3 CACHE STRING "" FORCE)
