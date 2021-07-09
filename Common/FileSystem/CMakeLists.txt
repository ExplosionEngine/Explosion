file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME FileSystem
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)
