file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Mirror
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
)
