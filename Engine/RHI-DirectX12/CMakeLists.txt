file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI-DirectX12
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    PRIVATE_INC ${DirectXHeaders_INC}
    LIB RHI d3d12 dxgi
)
