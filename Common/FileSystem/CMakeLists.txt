file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME FileSystem
    TYPE STATIC
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME FileSystemTest
    TYPE STATIC
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS FileSystem ${googletest_LIBS}
)


file(GLOB_RECURSE DEMO_SOURCES Src/*.cpp Sample/*.cpp)
exp_add_executable(
    NAME FileSystemSample
    SRCS ${DEMO_SOURCES}
    INC_DIRS Include
)
