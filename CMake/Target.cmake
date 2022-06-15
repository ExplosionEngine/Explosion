option(BUILD_TEST "Build unit tests" ON)
option(BUILD_SAMPLE "Build sample" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Api CACHE PATH "" FORCE)
set(BASIC_LIBS Common CACHE STRING "" FORCE)
set(BASIC_TEST_LIBS googletest CACHE STRING "" FORCE)

function(CombineRuntimeDependencies)
    cmake_parse_arguments(PARAMS "" "NAME" "RUNTIME_DEP" ${ARGN})

    get_target_property(RESULT ${PARAMS_NAME} RUNTIME_DEP)
    if ("${RESULT}" STREQUAL "RESULT-NOTFOUND")
        set(RESULT ${PARAMS_RUNTIME_DEP})
    else()
        list(APPEND RESULT ${PARAMS_RUNTIME_DEP})
    endif()
    set_target_properties(
        ${PARAMS_NAME} PROPERTIES
        RUNTIME_DEP "${RESULT}"
    )
endfunction()

function(LinkLibraries)
    cmake_parse_arguments(PARAMS "" "NAME" "LIB" ${ARGN})

    foreach(L ${PARAMS_LIB})
        if (TARGET ${L})
            get_target_property(3RD_TYPE ${L} 3RD_TYPE)
            if (${3RD_TYPE} STREQUAL "3RD_TYPE-NOTFOUND")
                target_link_libraries(${PARAMS_NAME} ${L})
            else()
                get_target_property(${L}_INCLUDE ${L} INCLUDE)
                get_target_property(${L}_LINK ${L} LINK)
                get_target_property(${L}_LIB ${L} LIB)

                if (NOT ("${${L}_INCLUDE}" STREQUAL "${L}_INCLUDE-NOTFOUND"))
                    target_include_directories(${PARAMS_NAME} PUBLIC ${${L}_INCLUDE})
                endif()
                if (NOT ("${${L}_LINK}" STREQUAL "${L}_LINK-NOTFOUND"))
                    target_link_directories(${PARAMS_NAME} PUBLIC ${${L}_LINK})
                endif()
                if (NOT ("${${L}_LIB}" STREQUAL "${L}_LIB-NOTFOUND"))
                    target_link_libraries(${PARAMS_NAME} ${${L}_LIB})
                endif()

                if (${3RD_TYPE} STREQUAL "CMakeProject")
                    add_dependencies(${PARAMS_NAME} ${L})
                endif()
            endif()

            get_target_property(${L}_RUNTIME_DEP ${L} RUNTIME_DEP)
            if (NOT ("${${L}_RUNTIME_DEP}" STREQUAL "${L}_RUNTIME_DEP-NOTFOUND"))
                CombineRuntimeDependencies(
                    NAME ${PARAMS_NAME}
                    RUNTIME_DEP "${${L}_RUNTIME_DEP}"
                )
            endif()
        else()
            target_link_libraries(${PARAMS_NAME} ${L})
        endif()
    endforeach()
endfunction()

function(LinkBasicLibs)
    cmake_parse_arguments(PARAMS "" "NAME" "LIB" ${ARGN})

    foreach(L ${PARAMS_LIB})
        if (NOT (${PARAMS_NAME} STREQUAL ${L}))
            LinkLibraries(
                NAME ${PARAMS_NAME}
                LIB ${L}
            )
        endif()
    endforeach()
endfunction()

function(AddRuntimeDependenciesCopyCommand)
    cmake_parse_arguments(PARAMS "" "NAME" "" ${ARGN})

    get_target_property(RUNTIME_DEP ${PARAMS_NAME} RUNTIME_DEP)
    if (NOT ("${RUNTIME_DEP}" STREQUAL "RUNTIME_DEP-NOTFOUND"))
        foreach(R ${RUNTIME_DEP})
            add_custom_command(
                TARGET ${PARAMS_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${R} $<TARGET_FILE_DIR:${PARAMS_NAME}>
            )
        endforeach()
    endif()
endfunction()

function(ExpandResourcePathExpression)
    cmake_parse_arguments(PARAMS "" "INPUT;OUTPUT_SRC;OUTPUT_DST" "" ${ARGN})

    string(REPLACE "->" ";" TEMP ${PARAMS_INPUT})
    list(GET TEMP 0 SRC)
    list(GET TEMP 1 DST)

    set(${PARAMS_OUTPUT_SRC} ${SRC} PARENT_SCOPE)
    set(${PARAMS_OUTPUT_DST} ${DST} PARENT_SCOPE)
endfunction()

function(AddResourcesCopyCommand)
    cmake_parse_arguments(PARAMS "" "NAME" "RES" ${ARGN})

    foreach(R ${PARAMS_RES})
        ExpandResourcePathExpression(
            INPUT ${R}
            OUTPUT_SRC SRC
            OUTPUT_DST DST
        )
        add_custom_command(
            TARGET ${PARAMS_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>/${DST}
        )
    endforeach()
endfunction()

function(AddExecutable)
    cmake_parse_arguments(PARAMS "SAMPLE" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES" ${ARGN})

    if ((DEFINED PARAMS_SAMPLE) AND (NOT ${BUILD_SAMPLE}))
        return()
    endif()

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
    LinkBasicLibs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS}
    )
    LinkLibraries(
        NAME ${PARAMS_NAME}
        LIB ${PARAMS_LIB}
    )
    AddRuntimeDependenciesCopyCommand(
        NAME ${PARAMS_NAME}
    )
    AddResourcesCopyCommand(
        NAME ${PARAMS_NAME}
        RES ${PARAMS_RES}
    )
    if (DEFINED PARAMS_DEP_TARGET)
        add_dependencies(${PARAMS_NAME} ${PARAMS_DEP_TARGET})
    endif()

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
    LinkBasicLibs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS}
    )
    LinkLibraries(
        NAME ${PARAMS_NAME}
        LIB ${PARAMS_LIB}
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

    cmake_parse_arguments(PARAMS "" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES" ${ARGN})

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
    LinkBasicLibs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS} ${BASIC_TEST_LIBS}
    )
    LinkLibraries(
        NAME ${PARAMS_NAME}
        LIB ${PARAMS_LIB}
    )
    AddRuntimeDependenciesCopyCommand(
        NAME ${PARAMS_NAME}
    )
    AddResourcesCopyCommand(
        NAME ${PARAMS_NAME}
        RES ${PARAMS_RES}
    )
    if (DEFINED PARAMS_DEP_TARGET)
        add_dependencies(${PARAMS_NAME} ${PARAMS_DEP_TARGET})
    endif()

    add_test(
        NAME ${PARAMS_NAME}
        COMMAND ${PARAMS_NAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${PARAMS_NAME}>
    )
endfunction()
