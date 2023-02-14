file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME MirrorTool.Static
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB clipp llvm-clang Mirror
)

file(GLOB EXE_SOURCES ExeSrc/*.cpp)
AddExecutable(
    NAME MirrorTool
    SRC ${SOURCES}
    INC Include
    LIB clipp llvm-clang Mirror
)

AddTest(
    NAME MirrorTool.Test
    SRC Test/Main.cpp
    INC Include
    LIB MirrorTool.Static
    RES
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/MirrorToolInput.h->TestRes/MirrorToolInput.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/MirrorToolOutput.cpp->TestRes/MirrorToolOutput.cpp
        ${CMAKE_SOURCE_DIR}/Engine/Source/Mirror/Include/Mirror/Meta.h->TestRes/Mirror/Meta.h
)
