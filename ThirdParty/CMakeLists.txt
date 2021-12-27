# EnTT
AddThirdPartyPackage(
    NAME entt
    VERSION 3.8.1
)
set(entt_INC ${entt_SOURCE_DIR}/single_include CACHE PATH "" FORCE)

# GLM
AddThirdPartyPackage(
    NAME glm
    VERSION 0.9.9.8
    ARG -DGLM_TEST_ENABLE=OFF
)
set(glm_INC ${glm_SOURCE_DIR} CACHE PATH "" FORCE)

# Meta
AddThirdPartyPackage(
    NAME meta
    VERSION 1.4.9
    ARG -DBUILD_TESTING=OFF
)
set(meta_INC ${meta_SOURCE_DIR}/src CACHE PATH "" FORCE)

# DirectX 12 Headers
AddThirdPartyPackage(
    NAME DirectX-Headers
    VERSION 1.4.9
)
set(DirectX-Headers_INC ${DirectX-Headers_SOURCE_DIR}/include CACHE PATH "" FORCE)
