file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME FileSystem
    TYPE STATIC
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
    LIBS CommonBase
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME FileSystemTest
    TYPE STATIC
    WORKING_DIRECTORY ${PROJECT_RUNTIME_DIRECTORY}
    SRCS ${TEST_SOURCES}
    INC_DIRS ${googletest_INCS}
    LIBS FileSystem ${googletest_LIBS}
)


file(GLOB_RECURSE DEMO_SOURCES Sample/*.cpp)
exp_add_executable(
    NAME FileSystemSample
    SRCS ${DEMO_SOURCES}
    INC_DIRS Include
    LIBS FileSystem
)

set(FILESYSTEMSAMPLE_PATH ${PROJECT_RUNTIME_DIRECTORY}/FSSamplePath)

add_custom_command(
    TARGET FileSystemSample
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${FILESYSTEMSAMPLE_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FILESYSTEMSAMPLE_PATH}
)
