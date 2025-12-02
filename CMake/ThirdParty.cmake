function(exp_download_and_extract_3rd_package)
    set(options "")
    set(singleValueArgs URL SAVE_AS EXTRACT_TO)
    set(multiValueArgs HASH)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (EXISTS ${arg_SAVE_AS})
        message("found downloaded file for ${arg_URL} -> ${arg_SAVE_AS}")
    else()
        message("starting download package ${arg_URL}")
        file(
            DOWNLOAD
            ${arg_URL} ${arg_SAVE_AS}
            SHOW_PROGRESS
        )
    endif()

    if (DEFINED arg_HASH)
        exp_get_3rd_platform_value(
            OUTPUT platform_hash
            INPUT ${arg_HASH}
        )

        file(SHA256 ${arg_SAVE_AS} hash_value)
        if (NOT (${platform_hash} STREQUAL ${hash_value}))
            message(FATAL_ERROR "check hash value failed for file ${arg_SAVE_AS}, given ${platform_hash} actual ${hash_value}")
        endif ()
    endif()

    if (NOT EXISTS ${arg_EXTRACT_TO})
        file(
            ARCHIVE_EXTRACT
            INPUT ${arg_SAVE_AS}
            DESTINATION ${arg_EXTRACT_TO}
        )
    endif()
endfunction()

function(exp_expand_3rd_path_expression)
    set(options "")
    set(singleValueArgs OUTPUT SOURCE_DIR BINARY_DIR INSTALL_DIR)
    set(multiValueArgs INPUT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(i ${arg_INPUT})
        set(temp "${i}")
        if (DEFINED arg_SOURCE_DIR)
            string(REPLACE "$<SOURCE_DIR>" ${arg_SOURCE_DIR} temp ${temp})
        endif()
        if (DEFINED arg_BINARY_DIR)
            string(REPLACE "$<BINARY_DIR>" ${arg_BINARY_DIR} temp ${temp})
        endif()
        if (DEFINED arg_INSTALL_DIR)
            string(REPLACE "$<INSTALL_DIR>" ${arg_INSTALL_DIR} temp ${temp})
        endif()
        list(APPEND result ${temp})
    endforeach()

    set(${arg_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

function(exp_get_3rd_platform_value)
    set(options "")
    set(singleValueArgs OUTPUT)
    set(multiValueArgs INPUT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    set(platform_keywords "Windows;Darwin;Linux")

    set(has_keywords FALSE)
    foreach (i ${arg_INPUT})
        foreach (k ${platform_keywords})
            if (${i} STREQUAL ${k})
                set(has_keywords TRUE)
                break()
            endif()
        endforeach()

        if (${has_keywords})
            break()
        endif()
    endforeach()

    set(start_log FALSE)
    foreach (i ${arg_INPUT})
        if ((NOT ${start_log}) AND (${i} STREQUAL ${CMAKE_SYSTEM_NAME}))
            set(start_log TRUE)
            continue()
        endif()

        if (NOT ${start_log})
            continue()
        endif()

        set(end_log FALSE)
        foreach (k ${platform_keywords})
            if (${k} STREQUAL ${i})
                set(end_log TRUE)
                break()
            endif()
        endforeach()

        if (${end_log})
            break()
        endif()

        list(APPEND result ${i})
    endforeach ()

    if (${has_keywords})
        set(${arg_OUTPUT} ${result} PARENT_SCOPE)
    else()
        set(${arg_OUTPUT} ${arg_INPUT} PARENT_SCOPE)
    endif()
endfunction()

function(exp_add_3rd_header_only_package)
    set(options "")
    set(singleValueArgs NAME SOURCE_DIR INSTALL_DIR)
    set(multiValueArgs INSTALL_FILES INCLUDE)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${arg_NAME} INTERFACE)

    if (DEFINED arg_INSTALL_DIR AND DEFINED arg_INSTALL_FILES)
        foreach (install_file ${arg_INSTALL_FILES})
            if (IS_DIRECTORY ${arg_SOURCE_DIR}/${install_file})
                list(APPEND commands COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${arg_SOURCE_DIR}/${install_file} ${arg_INSTALL_DIR}/${install_file})
            else ()
                list(APPEND commands COMMAND ${CMAKE_COMMAND} -E copy_if_different ${arg_SOURCE_DIR}/${install_file} ${arg_INSTALL_DIR}/${install_file})
            endif ()
        endforeach ()

        add_custom_target(
            ${arg_NAME}.Install
            COMMAND ${commands}
        )
        add_dependencies(${arg_NAME} ${arg_NAME}.Install)
    endif ()

    if (DEFINED arg_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${arg_INCLUDE}
            OUTPUT r_include
            SOURCE_DIR ${arg_SOURCE_DIR}
            INSTALL_DIR ${arg_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            OUTPUT p_include
            INPUT ${r_include}
        )
        target_include_directories(
            ${arg_NAME}
            INTERFACE ${p_include}
        )
    endif ()
endfunction()

function(exp_add_3rd_binary_package)
    set(options "")
    set(singleValueArgs NAME SOURCE_DIR)
    set(multiValueArgs INCLUDE LINK LIB RUNTIME_DEP)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${arg_NAME} INTERFACE)

    if (DEFINED arg_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${arg_INCLUDE}
            OUTPUT r_include
            SOURCE_DIR ${arg_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_include}
            OUTPUT p_include
        )
        target_include_directories(
            ${arg_NAME}
            INTERFACE ${p_include}
        )
    endif()

    if (DEFINED arg_LINK)
        exp_expand_3rd_path_expression(
            INPUT ${arg_LINK}
            OUTPUT r_link
            SOURCE_DIR ${arg_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_link}
            OUTPUT p_link
        )
        target_link_directories(
            ${arg_NAME}
            INTERFACE ${p_link}
        )
    endif()

    if (DEFINED arg_LIB)
        exp_expand_3rd_path_expression(
            INPUT ${arg_LIB}
            OUTPUT r_lib
            SOURCE_DIR ${arg_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            OUTPUT p_lib
            INPUT ${r_lib}
        )
        target_link_libraries(
            ${arg_NAME}
            INTERFACE "${p_lib}"
        )
    endif()

    if (DEFINED arg_RUNTIME_DEP)
        exp_expand_3rd_path_expression(
            INPUT ${arg_RUNTIME_DEP}
            OUTPUT r_runtime_dep
            SOURCE_DIR ${arg_SOURCE_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_runtime_dep}
            OUTPUT p_runtime_dep
        )
        set_target_properties(
            ${arg_NAME} PROPERTIES
            RUNTIME_DEP "${p_runtime_dep}"
        )
    endif()
endfunction()

function(exp_add_3rd_cmake_package)
    set(options "")
    set(singleValueArgs NAME SOURCE_DIR BINARY_DIR INSTALL_DIR)
    set(multiValueArgs CMAKE_ARG INCLUDE LINK LIB RUNTIME_DEP)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT ${generator_is_multi_config})
        set(build_type_args -DCMAKE_BUILD_TYPE=Release)
    endif ()

    ExternalProject_Add(
        ${arg_NAME}.External
        SOURCE_DIR ${arg_SOURCE_DIR}
        BINARY_DIR ${arg_BINARY_DIR}
        CMAKE_ARGS ${build_type_args} -DCMAKE_INSTALL_PREFIX=${arg_INSTALL_DIR} ${arg_CMAKE_ARG}
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release -j 16
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config Release
    )
    add_library(${arg_NAME} INTERFACE)
    add_dependencies(${arg_NAME} ${arg_NAME}.External)

    if (DEFINED arg_INCLUDE)
        exp_expand_3rd_path_expression(
            INPUT ${arg_INCLUDE}
            OUTPUT r_include
            SOURCE_DIR ${arg_SOURCE_DIR}
            BINARY_DIR ${arg_BINARY_DIR}
            INSTALL_DIR ${arg_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_include}
            OUTPUT p_include
        )
        target_include_directories(
            ${arg_NAME}
            INTERFACE ${p_include}
        )
    endif()

    if (DEFINED arg_LINK)
        exp_expand_3rd_path_expression(
            INPUT ${arg_LINK}
            OUTPUT r_link
            SOURCE_DIR ${arg_SOURCE_DIR}
            BINARY_DIR ${arg_BINARY_DIR}
            INSTALL_DIR ${arg_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_link}
            OUTPUT p_link
        )
        target_link_directories(
            ${arg_NAME}
            INTERFACE ${p_link}
        )
    endif()

    if (DEFINED arg_LIB)
        exp_expand_3rd_path_expression(
            INPUT ${arg_LIB}
            OUTPUT r_lib
            SOURCE_DIR ${arg_SOURCE_DIR}
            BINARY_DIR ${arg_BINARY_DIR}
            INSTALL_DIR ${arg_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_lib}
            OUTPUT p_lib
        )
        target_link_libraries(
            ${arg_NAME}
            INTERFACE "${p_lib}"
        )
    endif()

    if (DEFINED arg_RUNTIME_DEP)
        exp_expand_3rd_path_expression(
            INPUT ${arg_RUNTIME_DEP}
            OUTPUT r_runtime_dep
            SOURCE_DIR ${arg_SOURCE_DIR}
            BINARY_DIR ${arg_BINARY_DIR}
            INSTALL_DIR ${arg_INSTALL_DIR}
        )
        exp_get_3rd_platform_value(
            INPUT ${r_runtime_dep}
            OUTPUT p_runtime_dep
        )
        set_target_properties(
            ${arg_NAME} PROPERTIES
            RUNTIME_DEP "${p_runtime_dep}"
        )
    endif()
endfunction()

function(exp_add_3rd_alias_package)
    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs LIB)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${arg_NAME} INTERFACE)
    target_link_libraries(
        ${arg_NAME}
        INTERFACE "${arg_LIB}"
    )
endfunction()
