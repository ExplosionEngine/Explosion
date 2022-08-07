file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Engine
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
)
