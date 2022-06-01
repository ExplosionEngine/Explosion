file(GLOB SOURCES Src/Static/*.cpp)
AddLibrary(
    NAME MetaTool.Static
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Common LLVM
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
    LIB MetaTool.Static googletest
    WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Test
)
