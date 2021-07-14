file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME AssetsCommon
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)
