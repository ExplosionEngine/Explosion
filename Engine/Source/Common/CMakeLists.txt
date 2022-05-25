file(GLOB SOURCES Src/*.cpp)

AddLibrary(
    NAME Common
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB debugbreak
)

AddTest(
    NAME MetaTest
    SRC Test/Meta.cpp
    LIB Common
)
