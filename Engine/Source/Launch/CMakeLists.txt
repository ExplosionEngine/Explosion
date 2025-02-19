file(GLOB_RECURSE SOURCES Src/*.cpp)
AddLibrary(
    NAME Launch
    TYPE STATIC
    SRC ${SOURCES}
    PRIVATE_INC Include
    LIB glfw Runtime
)