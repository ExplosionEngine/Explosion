file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Launch
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    PUBLIC_LIB Runtime
    PUBLIC_MERGE_LIB glfw
)