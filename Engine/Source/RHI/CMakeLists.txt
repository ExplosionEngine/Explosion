file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Core
)
