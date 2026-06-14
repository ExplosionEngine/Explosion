file(GLOB_RECURSE sources Src/*.cpp)
exp_add_library(
    NAME Launch
    TYPE STATIC
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Runtime glfw
)