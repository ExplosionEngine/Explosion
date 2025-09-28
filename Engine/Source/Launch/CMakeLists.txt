file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Launch
    TYPE STATIC
    SRC ${SOURCES}
    PRIVATE_INC Include
    LIB glfw Runtime
)