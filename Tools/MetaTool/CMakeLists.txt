file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME MetaTool.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB llvm-clang
)

file(GLOB SOURCES ExeSrc/*.cpp)
AddExecutable(
    NAME MetaTool
    SRC ${SOURCES}
    LIB MetaTool.Static clipp
)

AddTest(
    META
    NAME MetaTool.Test
    SRC Test/Main.cpp
    INC Test
    LIB MetaTool.Static
    RES
        ${CMAKE_SOURCE_DIR}/Engine/Source/Common/Include/Common/Meta.h->Test/MetaTool/Include/Common/Meta.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/ClassTest.h->Test/MetaTool/ClassTest.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/NamespaceTest.h->Test/MetaTool/NamespaceTest.h
)
