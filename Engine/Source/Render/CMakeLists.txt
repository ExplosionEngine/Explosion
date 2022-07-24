file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Render
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
)
