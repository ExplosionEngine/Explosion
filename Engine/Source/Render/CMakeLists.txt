file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME Render.Static
    TYPE STATIC
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Core RHI
    PUBLIC_MERGE_LIB dxc::dxc spirv-cross::spirv-cross
)

file(GLOB shader_sources SharedSrc/*.cpp)
exp_add_library(
    NAME Render
    TYPE SHARED
    SRC ${shader_sources}
    PUBLIC_LIB Render.Static
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Render.Test
    SRC ${test_sources}
    LIB RHI Render.Static
    DEP_TARGET RHI-Dummy
)
