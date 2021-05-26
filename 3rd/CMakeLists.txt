add_subdirectory(glfw)

if (${EXP_BUILD_TEST})
    add_subdirectory(googletest)
endif()