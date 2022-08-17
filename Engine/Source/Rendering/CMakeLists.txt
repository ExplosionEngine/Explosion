file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Rendering
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Render
)
