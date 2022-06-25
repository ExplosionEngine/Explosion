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
    NAME Shader.Test
    SRC ${SOURCES}
    LIB Shader
)
