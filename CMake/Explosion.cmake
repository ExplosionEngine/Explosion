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
        if ("${PARAMS_TYPE}" STREQUAL "SHARED")
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

function(exp_compare_3rd_version)
    cmake_parse_arguments(
        PARAMS
        ""
        "CURRENT_VERSION;REQUIRED_MIN_VERSION"
        ""
        ${ARGN}
    )

    string(LENGTH ${PARAMS_CURRENT_VERSION} CURRENT_VERSION_LEN)
    string(LENGTH ${PARAMS_REQUIRED_MIN_VERSION} MINIMUM_VERSION_LEN)
    string(SUBSTRING ${PARAMS_CURRENT_VERSION} 1 ${CURRENT_VERSION_LEN} PARAMS_CURRENT_VERSION)
    string(SUBSTRING ${PARAMS_REQUIRED_MIN_VERSION} 1 ${MINIMUM_VERSION_LEN} PARAMS_REQUIRED_MIN_VERSION)
    string(REPLACE "." ";" CURRENT_VERSION_LIST ${PARAMS_CURRENT_VERSION})
    string(REPLACE "." ";" MINIMUM_VERSION_LIST ${PARAMS_REQUIRED_MIN_VERSION})

    LIST(GET CURRENT_VERSION_LIST 0 CURRENT_VERSION_ARGS0)
    LIST(GET MINIMUM_VERSION_LIST 0 MINIMUM_VERSION_ARGS0)
    LIST(GET CURRENT_VERSION_LIST -1 CURRENT_VERSION_ARGS1)
    LIST(GET MINIMUM_VERSION_LIST -1 MINIMUM_VERSION_ARGS1)

    if (${CURRENT_VERSION_ARGS0} GREATER ${MINIMUM_VERSION_ARGS0})
        message(STATUS "required min 3rd party package version: ${PARAMS_REQUIRED_MIN_VERSION}, but current 3rd party package version is: ${PARAMS_CURRENT_VERSION}")
    elseif((${CURRENT_VERSION_ARGS0} EQUAL ${MINIMUM_VERSION_ARGS0}) AND (NOT ${CURRENT_VERSION_ARGS1} LESS ${MINIMUM_VERSION_ARGS1}))
        message(STATUS "required min 3rd party package version: ${PARAMS_REQUIRED_MIN_VERSION}, but current 3rd party package version is: ${PARAMS_CURRENT_VERSION}")
    else()
        message(FATAL_ERROR
            "3rd party package version ${PARAMS_REQUIRED_MIN_VERSION} or higher is required"
            "Your current 3rd party version is ${PARAMS_CURRENT_VERSION}"
            "Please get latest 3rd party at https://github.com/ExplosionEngine/Explosion3rdParty/releases")
    endif ()
endfunction()
