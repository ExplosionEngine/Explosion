file(GLOB SOURCES Src/*.cpp)
exp_add_executable(
    NAME AssetsCompiler
    SRCS ${SOURCES}
    INC_DIRS Include ${gflags_INCS}
    LIBS ${gflags_LIBS}
)
