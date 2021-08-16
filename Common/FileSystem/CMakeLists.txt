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


file(GLOB_RECURSE DEMO_SOURCES Sample/*.cpp)
exp_add_executable(
    NAME FileSystemSample
    SRCS ${DEMO_SOURCES}
    INC_DIRS Include
    LIBS FileSystem
)

set(FILESYSTEMTEST_PATH ${PROJECT_RUNTIME_DIRECTORY}/FSTestPath)
set(FILESYSTEMSAMPLE_PATH ${PROJECT_RUNTIME_DIRECTORY}/FSSamplePath)

add_custom_command(
    TARGET FileSystemTest
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${FILESYSTEMTEST_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FILESYSTEMTEST_PATH}
)

add_custom_command(
    TARGET FileSystemSample
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${FILESYSTEMSAMPLE_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FILESYSTEMSAMPLE_PATH}
)
