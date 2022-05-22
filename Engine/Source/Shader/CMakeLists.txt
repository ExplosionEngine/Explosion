file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Shader
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB dxc
)
