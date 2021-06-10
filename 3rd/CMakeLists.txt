add_subdirectory(glfw)

if (${EXP_BUILD_TEST})
    add_subdirectory(googletest)
endif()

set(TF_BUILD_TESTS false)
set(TF_BUILD_EXAMPLES false)
add_subdirectory(taskflow)
