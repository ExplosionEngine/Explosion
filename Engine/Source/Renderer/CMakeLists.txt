file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Renderer
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Render
)
