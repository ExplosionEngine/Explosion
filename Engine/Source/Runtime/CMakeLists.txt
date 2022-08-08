file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Runtime
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB EnTT
)
