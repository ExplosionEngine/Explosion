function(exp_add_executable)
    cmake_parse_arguments(
        PARAMS
        ""
        "NAME"
        "SRCS;INC_DIRS;LIBS"
        ${ARGN}
    )

    if (${EXP_VERBOSE_INFO})
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
    if (NOT ${EXP_BUILD_TEST})
        return()
    endif()

    cmake_parse_arguments(
        PARAMS
        ""
        "NAME;WORKING_DIR"
        "SRCS;INC_DIRS;LIBS"
        ${ARGN}
    )

    if (${EXP_VERBOSE_INFO})
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
        "SRCS;PRIVATE_INC_DIRS;PUBLIC_INC_DIRS;LIBS;MSVC_IGNORE_LIBS"
        ${ARGN}
    )

    if (NOT DEFINED ${PARAMS_TYPE})
        set(PARAMS_TYPE "STATIC")
    endif()

    if (${EXP_VERBOSE_INFO})
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

    if (${MSVC})
        if (${PARAMS_TYPE} STREQUAL "SHARED")
            if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                target_compile_options(${PARAMS_NAME} PUBLIC "/MDd")
            endif()
            if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
                target_compile_options(${PARAMS_NAME} PUBLIC "/MD")
            endif()
        else()
            if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
                target_compile_options(${PARAMS_NAME} PUBLIC "/MTd")
            endif()
            if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
                target_compile_options(${PARAMS_NAME} PUBLIC "/MT")
            endif()
        endif()
    endif()
endfunction()

function(exp_external_library)
    cmake_parse_arguments(
        PARAMS
        ""
        "NAME"
        ""
        ${ARGN}
    )

    if (EXISTS "${EXP_3RD_ROOT}/${PARAMS_NAME}/Include")
        set("${PARAMS_NAME}_INCS" "${EXP_3RD_ROOT}/${PARAMS_NAME}/Include" CACHE STRING "include dirs of ${PARAMS_NAME} library")
    endif()
    if (EXISTS "${EXP_3RD_ROOT}/${PARAMS_NAME}/Lib")
        if (${WIN32})
            set(LIBS "${EXP_3RD_ROOT}/${PARAMS_NAME}/Lib/${CMAKE_BUILD_TYPE}/*.lib")
        else(${APPLE})
            file(GLOB LIBS "${EXP_3RD_ROOT}/${PARAMS_NAME}/Lib/${CMAKE_BUILD_TYPE}/*.a")
        endif ()
        set("${PARAMS_NAME}_LIBS" ${LIBS} CACHE STRING "include dirs of ${PARAMS_NAME} library")
    endif()
endfunction()
