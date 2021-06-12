add_subdirectory(glfw)

if (${EXP_BUILD_TEST})
    add_subdirectory(googletest)
endif()

set(TF_BUILD_TESTS OFF CACHE BOOL "build taskflow tests")
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "build taskflow samples")
add_subdirectory(taskflow)
