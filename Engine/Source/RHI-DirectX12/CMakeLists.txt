file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME RHI-DirectX12
    TYPE SHARED
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB RHI d3d12
    PUBLIC_MERGE_LIB Microsoft::DirectX-Headers dxgi d3dcompiler
)
