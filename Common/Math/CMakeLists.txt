file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME Math
    SRCS ${SOURCES}
    PRIVATE_INC_DIRS ${PROJECT_ROOT}/3rd/glm
    PUBLIC_INC_DIRS Include
)
