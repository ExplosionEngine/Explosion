option(BUILD_TEST "Build unit tests" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Api CACHE PATH "" FORCE)

function(AddExecutable)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;INC;LINK;LIB;RUNTIME_DEP" ${ARGN})

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_LINK}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )

    foreach(D ${PARAMS_RUNTIME_DEP})
        add_custom_command(
            TARGET ${PARAMS_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${D} $<TARGET_FILE_DIR:${PARAMS_NAME}>
        )
    endforeach()

    if (${MSVC})
        set_target_properties(${PARAMS_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
    endif()
endfunction()

function(AddLibrary)
    cmake_parse_arguments(PARAMS "" "NAME;TYPE" "SRC;PRIVATE_INC;PUBLIC_INC;PRIVATE_LINK;LIB" ${ARGN})

    add_library(
        ${PARAMS_NAME}
        ${PARAMS_TYPE}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_PRIVATE_INC}
        PUBLIC ${PARAMS_PUBLIC_INC}
    )
    target_link_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_PRIVATE_LINK}
        PUBLIC ${PARAMS_PUBLIC_LINK}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )

    if (${MSVC})
        target_compile_options(
            ${PARAMS_NAME}
            PUBLIC $<IF:$<STREQUAL:${PARAMS_TYPE},"SHARED">,/MD$<$<CONFIG:Debug>:d>,/MT$<$<CONFIG:Debug>:d>>
        )
    endif()

    if ("${PARAMS_TYPE}" STREQUAL "SHARED")
        string(TOUPPER ${PARAMS_NAME}_API API_NAME)
        string(REPLACE "-" "/" API_DIR ${PARAMS_NAME})

        generate_export_header(
            ${PARAMS_NAME}
            EXPORT_MACRO_NAME ${API_NAME}
            EXPORT_FILE_NAME ${API_HEADER_DIR}/${PARAMS_NAME}/${API_DIR}/Api.h
        )
        target_include_directories(
            ${PARAMS_NAME}
            PUBLIC ${API_HEADER_DIR}/${PARAMS_NAME}
        )
    endif()
endfunction()

function(AddTest)
    if (NOT ${BUILD_TEST})
        return()
    endif()

    cmake_parse_arguments(PARAMS "" "NAME;WORKING_DIR" "SRC;INC;LINK;LIB" ${ARGN})

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_LINK}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )
    add_test(
        NAME ${PARAMS_NAME}
        COMMAND ${PARAMS_NAME}
        WORKING_DIRECTORY ${PARAM_WORKING_DIR}
    )
endfunction()
