file(GLOB_RECURSE SOURCES Src/*.cpp)
AddExecutable(
    NAME Editor
    SRC ${SOURCES}
    INC Include
    LIB Qt
)