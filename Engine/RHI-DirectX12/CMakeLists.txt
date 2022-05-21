file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI-DirectX12
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI DirectXHeaders d3d12 dxgi d3dcompiler
)
