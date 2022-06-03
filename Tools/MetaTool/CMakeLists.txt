file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME MetaTool
    SRC ${SOURCES}
    INC Include
    LIB Common LLVM clipp
)
