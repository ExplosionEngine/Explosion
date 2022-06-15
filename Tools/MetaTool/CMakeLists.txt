file(GLOB SOURCES Src/Static/*.cpp)
AddLibrary(
    NAME MetaTool.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB llvm-clang
)

file(GLOB SOURCES Src/Exe/*.cpp)
AddExecutable(
    NAME MetaTool
    SRC ${SOURCES}
    LIB MetaTool.Static clipp
)

AddTest(
    NAME MetaTool.Test
    SRC Test/Main.cpp
    LIB MetaTool.Static
    RES
        ${CMAKE_SOURCE_DIR}/Engine/Source/Common/Include/Common/Meta.h->Test/MetaTool/Include/Common/Meta.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/StructClassTest.h->Test/MetaTool/StructClassTest.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/NamespaceTest.h->Test/MetaTool/NamespaceTest.h
)
