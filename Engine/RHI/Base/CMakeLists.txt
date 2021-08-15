file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME RHI
    SRCS ${SOURCES}
    PUBLIC_INC_DIRS Include
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME RHITest
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${TEST_SOURCES}
    INC_DIRS Include ${googletest_INCS}
    LIBS RHI ${googletest_LIBS}
)

if (${EXP_BUILD_SAMPLES})
    add_subdirectory(Samples)
endif()
