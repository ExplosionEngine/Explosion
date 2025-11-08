function(exp_download_and_extract_3rd_package)
    cmake_parse_arguments(PARAMS "" "URL;SAVE_AS;EXTRACT_TO" "ARG;HASH" ${ARGN})

    if (EXISTS ${PARAMS_SAVE_AS})
        message("found downloaded file for ${PARAMS_URL} -> ${PARAMS_SAVE_AS}")
    else()
        message("starting download package ${PARAMS_URL}")
        file(
            DOWNLOAD
            ${PARAMS_URL} ${PARAMS_SAVE_AS}
            SHOW_PROGRESS
        )
    endif()

    if (DEFINED PARAMS_HASH)
        exp_get_3rd_platform_value(
            OUTPUT PLATFORM_HASH
            INPUT ${PARAMS_HASH}
        )

        file(SHA256 ${PARAMS_SAVE_AS} HASH_VALUE)
        if (NOT (${PLATFORM_HASH} STREQUAL ${HASH_VALUE}))
            message(FATAL_ERROR "check hash value failed for file ${PARAMS_SAVE_AS}, given ${PLATFORM_HASH} actual ${HASH_VALUE}")
        endif ()
    endif()

    if (NOT EXISTS ${PARAMS_EXTRACT_TO})
        file(
            ARCHIVE_EXTRACT
            INPUT ${PARAMS_SAVE_AS}
            DESTINATION ${PARAMS_EXTRACT_TO}
        )
    endif()
endfunction()

function(exp_expand_3rd_path_expression)
    cmake_parse_arguments(PARAMS "" "OUTPUT;SOURCE_DIR;BINARY_DIR;INSTALL_DIR" "INPUT" ${ARGN})

    foreach(I ${PARAMS_INPUT})
        set(TEMP "${I}")
        if (DEFINED PARAMS_SOURCE_DIR)
            string(REPLACE "$<SOURCE_DIR>" ${PARAMS_SOURCE_DIR} TEMP ${TEMP})
        endif()
        if (DEFINED PARAMS_BINARY_DIR)
            string(REPLACE "$<BINARY_DIR>" ${PARAMS_BINARY_DIR} TEMP ${TEMP})
        endif()
        if (DEFINED PARAMS_INSTALL_DIR)
            string(REPLACE "$<INSTALL_DIR>" ${PARAMS_INSTALL_DIR} TEMP ${TEMP})
        endif()
        list(APPEND RESULT ${TEMP})
    endforeach()

    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(exp_get_3rd_platform_value)
    cmake_parse_arguments(PARAMS "" "OUTPUT" "INPUT" ${ARGN})

    set(PLATFORM_KEYWORDS "Windows;Darwin;Linux")

    set(HAS_KEYWORDS FALSE)
    foreach (I ${PARAMS_INPUT})
        foreach (K ${PLATFORM_KEYWORDS})
            if (${I} STREQUAL ${K})
                set(HAS_KEYWORDS TRUE)
                break()
            endif()
        endforeach()

        if (${HAS_KEYWORDS})
            break()
        endif()
    endforeach()

    set(START_LOG FALSE)
    foreach (I ${PARAMS_INPUT})
        if ((NOT ${START_LOG}) AND (${I} STREQUAL ${CMAKE_SYSTEM_NAME}))
            set(START_LOG TRUE)
            continue()
        endif()

        if (NOT ${START_LOG})
            continue()
        endif()

        set(END_LOG FALSE)
        foreach (K ${PLATFORM_KEYWORDS})
            if (${K} STREQUAL ${I})
                set(END_LOG TRUE)
                break()
            endif()
        endforeach()

        if (${END_LOG})
            break()
        endif()

        list(APPEND RESULT ${I})
    endforeach ()

    if (${HAS_KEYWORDS})
        set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
    else()
        set(${PARAMS_OUTPUT} ${PARAMS_INPUT} PARENT_SCOPE)
    endif()
endfunction()

function(exp_add_3rd_header_only_package)
    cmake_parse_arguments(PARAMS "" "NAME;SOURCE_DIR;INSTALL_DIR" "INSTALL_FILES;INCLUDE" ${ARGN})

    add_library(${PARAMS_NAME} INTERFACE)

    if (DEFINED PARAMS_INSTALL_DIR AND DEFINED PARAMS_INSTALL_FILES)
        foreach (INSTALL_FILE ${PARAMS_INSTALL_FILES})
            if (IS_DIRECTORY ${PARAMS_SOURCE_DIR}/${INSTALL_FILE})
                list(APPEND COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${PARAMS_SOURCE_DIR}/${INSTALL_FILE} ${PARAMS_INSTALL_DIR}/${INSTALL_FILE})
            else ()
                list(APPEND COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PARAMS_SOURCE_DIR}/${INSTALL_FILE} ${PARAMS_INSTALL_DIR}/${INSTALL_FILE})
            endif ()
        endforeach ()

        add_custom_target(
            ${PARAMS_NAME}.Install
            COMMAND ${COMMANDS}
        )
        add_dependencies(${PARAMS_NAME} ${PARAMS_NAME}.Install)
    endif ()

    if (DEFINED PARAMS_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
            INSTALL_DIR ${PARAMS_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            OUTPUT P_INCLUDE
            INPUT ${R_INCLUDE}
        )
        target_include_directories(
            ${PARAMS_NAME}
            INTERFACE ${P_INCLUDE}
        )
    endif ()
endfunction()

function(exp_add_3rd_binary_package)
    cmake_parse_arguments(PARAMS "" "NAME;SOURCE_DIR" "INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    add_library(${PARAMS_NAME} INTERFACE)

    if (DEFINED PARAMS_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_INCLUDE}
            OUTPUT P_INCLUDE
        )
        target_include_directories(
            ${PARAMS_NAME}
            INTERFACE ${P_INCLUDE}
        )
    endif()

    if (DEFINED PARAMS_LINK)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_LINK}
            OUTPUT R_LINK
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_LINK}
            OUTPUT P_LINK
        )
        target_link_directories(
            ${PARAMS_NAME}
            INTERFACE ${P_LINK}
        )
    endif()

    if (DEFINED PARAMS_LIB)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_LIB}
            OUTPUT R_LIB
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            OUTPUT P_LIB
            INPUT ${R_LIB}
        )
        target_link_libraries(
            ${PARAMS_NAME}
            INTERFACE "${P_LIB}"
        )
    endif()

    if (DEFINED PARAMS_RUNTIME_DEP)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_RUNTIME_DEP}
            OUTPUT R_RUNTIME_DEP
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_RUNTIME_DEP}
            OUTPUT P_RUNTIME_DEP
        )
        set_target_properties(
            ${PARAMS_NAME} PROPERTIES
            BUILD_RUNTIME_DEP "${P_RUNTIME_DEP}"
        )
    endif()
endfunction()

function(exp_add_3rd_cmake_package)
    cmake_parse_arguments(PARAMS "" "NAME;SOURCE_DIR;BINARY_DIR;INSTALL_DIR" "CMAKE_ARG;INCLUDE;LINK;LIB;RUNTIME_DEP" ${ARGN})

    if (NOT ${GENERATOR_IS_MULTI_CONFIG})
        set(CMAKE_BUILD_TYPE_ARGS -DCMAKE_BUILD_TYPE=Release)
    endif ()

    ExternalProject_Add(
        ${PARAMS_NAME}.External
        SOURCE_DIR ${PARAMS_SOURCE_DIR}
        BINARY_DIR ${PARAMS_BINARY_DIR}
        CMAKE_ARGS ${CMAKE_BUILD_TYPE_ARGS} -DCMAKE_INSTALL_PREFIX=${PARAMS_INSTALL_DIR} ${PARAMS_CMAKE_ARG}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release -j 16
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config Release
    )
    add_library(${PARAMS_NAME} INTERFACE)
    add_dependencies(${PARAMS_NAME} ${PARAMS_NAME}.External)

    if (DEFINED PARAMS_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_INCLUDE}
            OUTPUT R_INCLUDE
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
            BINARY_DIR ${PARAMS_BINARY_DIR}
            INSTALL_DIR ${PARAMS_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_INCLUDE}
            OUTPUT P_INCLUDE
        )
        target_include_directories(
            ${PARAMS_NAME}
            INTERFACE ${P_INCLUDE}
        )
    endif()

    if (DEFINED PARAMS_LINK)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_LINK}
            OUTPUT R_LINK
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
            BINARY_DIR ${PARAMS_BINARY_DIR}
            INSTALL_DIR ${PARAMS_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_LINK}
            OUTPUT P_LINK
        )
        target_link_directories(
            ${PARAMS_NAME}
            INTERFACE ${P_LINK}
        )
    endif()

    if (DEFINED PARAMS_LIB)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_LIB}
            OUTPUT R_LIB
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
            BINARY_DIR ${PARAMS_BINARY_DIR}
            INSTALL_DIR ${PARAMS_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_LIB}
            OUTPUT P_LIB
        )
        target_link_libraries(
            ${PARAMS_NAME}
            INTERFACE "${P_LIB}"
        )
    endif()

    if (DEFINED PARAMS_RUNTIME_DEP)
        exp_expand_3rd_path_expression(
            INPUT ${PARAMS_RUNTIME_DEP}
            OUTPUT R_RUNTIME_DEP
            SOURCE_DIR ${PARAMS_SOURCE_DIR}
            BINARY_DIR ${PARAMS_BINARY_DIR}
            INSTALL_DIR ${PARAMS_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${R_RUNTIME_DEP}
            OUTPUT P_RUNTIME_DEP
        )
        set_target_properties(
            ${PARAMS_NAME} PROPERTIES
            BUILD_RUNTIME_DEP "${P_RUNTIME_DEP}"
        )
    endif()
endfunction()

function(exp_add_3rd_alias_package)
    cmake_parse_arguments(PARAMS "" "NAME" "LIB" ${ARGN})

    add_library(${PARAMS_NAME} INTERFACE)
    target_link_libraries(
        ${PARAMS_NAME}
        INTERFACE "${PARAMS_LIB}"
    )
endfunction()
