file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Mirror
    TYPE STATIC
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)
