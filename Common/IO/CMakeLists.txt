file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME IO
    TYPE STATIC
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
    LIBS CommonBase
)
