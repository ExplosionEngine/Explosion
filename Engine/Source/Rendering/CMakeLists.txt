file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Rendering
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Render Core
)
