function(exp_add_executable)
    cmake_parse_arguments(
        PARAMS
        ""
        "NAME"
        "SRCS;INC_DIRS;LIBS"
        ${ARGN}
    )

    if (CMAKE_VERBOSE_INFO)
        message("")
        message("[exp_add_executable begin]")
        message("-name: ${PARAMS_NAME}")
        message("-sources: ${PARAMS_SRCS}")
        message("-include dirs: ${PARAMS_INC_DIRS}")
        message("-libs: ${PARAMS_LIBS}")
        message("[exp_add_executable end]")
        message("")
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRCS}
    )
    target_include_directories(${PARAMS_NAME} PRIVATE ${PARAMS_INC_DIRS})
    target_link_libraries(${PARAMS_NAME} ${PARAMS_LIBS})
endfunction()

function(exp_add_test)
    if (NOT BUILD_TEST)
        return()
    endif()

    cmake_parse_arguments(
        PARAMS
        ""
        "NAME;WORKING_DIR"
        "SRCS;INC_DIRS;LIBS"
        ${ARGN}
    )

    if (CMAKE_VERBOSE_INFO)
        message("")
        message("[exp_add_test begin]")
        message("-name: ${PARAMS_NAME}")
        message("-working dir: ${PARAMS_WORKING_DIR}")
        message("-sources: ${PARAMS_SRCS}")
        message("-include dirs: ${PARAMS_INC_DIRS}")
        message("-libs: ${PARAMS_LIBS}")
        message("[exp_add_test end]")
        message("")
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRCS}
    )
    target_include_directories(${PARAMS_NAME} PRIVATE ${PARAMS_INC_DIRS})
    target_link_libraries(${PARAMS_NAME} ${PARAMS_LIBS})
    add_test(
        NAME ${PARAMS_NAME}
        COMMAND ${PARAMS_NAME}
        WORKING_DIRECTORY ${PARAMS_WORKING_DIR}
    )
endfunction()

function(exp_add_library)
    cmake_parse_arguments(
        PARAMS
        ""
        "NAME;TYPE"
        "SRCS;PRIVATE_INC_DIRS;PUBLIC_INC_DIRS;LIBS"
        ${ARGN}
    )

    if (CMAKE_VERBOSE_INFO)
        message("")
        message("[exp_add_library begin]")
        message("-name: ${PARAMS_NAME}")
        message("-type: ${PARAMS_TYPE}")
        message("-sources: ${PARAMS_SRCS}")
        message("-private include dirs: ${PARAMS_PRIVATE_INC_DIRS}")
        message("-public include dirs: ${PARAMS_PUBLIC_INC_DIRS}")
        message("-libs: ${PARAMS_LIBS}")
        message("[exp_add_library end]")
        message("")
    endif()

    add_library(
        ${PARAMS_NAME}
        ${PARAMS_TYPE}
        ${PARAMS_SRCS}
    )
    target_include_directories(${PARAMS_NAME} PRIVATE ${PARAMS_PRIVATE_INC_DIRS})
    target_include_directories(${PARAMS_NAME} PUBLIC ${PARAMS_PUBLIC_INC_DIRS})
    target_link_libraries(${PARAMS_NAME} ${PARAMS_LIBS})
endfunction()
