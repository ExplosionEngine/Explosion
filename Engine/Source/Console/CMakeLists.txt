file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Console
    SRC ${SOURCES}
    PUBLIC_INC Include
)
