file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME ShaderCompiler
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    PRIVATE_INC ${ShaderConductor_INC}
    PRIVATE_LINK ${ShaderConductor_LINK}
    LIB Common ${ShaderConductor_LIB}
)