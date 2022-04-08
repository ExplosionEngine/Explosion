file(GLOB SOURCES Src/*.cpp)

AddLibrary(
    NAME Common
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include ${debugbreak_INC}
)
