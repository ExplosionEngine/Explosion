file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME MirrorTool
    SRC ${SOURCES}
    INC Include
    LIB clipp llvm-clang Mirror
)
