file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Shader
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB dxc RHI GLM
)

file(GLOB SOURCES Test/*.cpp)
AddTest(
    META
    NAME Shader.Test
    SRC ${SOURCES}
    INC Test
    LIB Shader
)
