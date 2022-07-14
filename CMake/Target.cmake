option(BUILD_TEST "Build unit tests" ON)
option(BUILD_SAMPLE "Build sample" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Api CACHE PATH "" FORCE)
set(META_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Meta CACHE PATH "" FORCE)
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
        if (NOT (TARGET ${L}))
            target_link_libraries(${PARAMS_NAME} ${L})
            continue()
        endif()

        get_target_property(3RD_TYPE ${L} 3RD_TYPE)
        if (${3RD_TYPE} STREQUAL "3RD_TYPE-NOTFOUND")
            target_link_libraries(${PARAMS_NAME} ${L})
        else()
            get_target_property(INCLUDE ${L} 3RD_INCLUDE)
            get_target_property(LINK ${L} 3RD_LINK)
            get_target_property(LIB ${L} 3RD_LIB)

            if (NOT ("${INCLUDE}" STREQUAL "INCLUDE-NOTFOUND"))
                target_include_directories(${PARAMS_NAME} PUBLIC ${INCLUDE})
            endif()
            if (NOT ("${LINK}" STREQUAL "LINK-NOTFOUND"))
                target_link_directories(${PARAMS_NAME} PUBLIC ${LINK})
            endif()
            if (NOT ("${LIB}" STREQUAL "LIB-NOTFOUND"))
                target_link_libraries(${PARAMS_NAME} ${LIB})
            endif()

            if (${3RD_TYPE} STREQUAL "CMakeProject")
                add_dependencies(${PARAMS_NAME} ${L})
            endif()
        endif()

        get_target_property(RUNTIME_DEP ${L} 3RD_RUNTIME_DEP)
        if (NOT ("${RUNTIME_DEP}" STREQUAL "RUNTIME_DEP-NOTFOUND"))
            CombineRuntimeDependencies(
                NAME ${PARAMS_NAME}
                RUNTIME_DEP "${RUNTIME_DEP}"
            )
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

function(GetTargetRuntimeDependenciesRecurse)
    cmake_parse_arguments(PARAMS "" "NAME;OUTPUT" "" ${ARGN})

    get_target_property(RUNTIME_DEP ${PARAMS_NAME} RUNTIME_DEP)
    if (NOT ("${RUNTIME_DEP}" STREQUAL "RUNTIME_DEP-NOTFOUND"))
        foreach(R ${RUNTIME_DEP})
            list(APPEND RESULT ${R})
        endforeach()
    endif()

    get_target_property(LINK_LIBRARIES ${PARAMS_NAME} LINK_LIBRARIES)
    if (NOT ("${LINK_LIBRARIES}" STREQUAL "LINK_LIBRARIES-NOTFOUND"))
        foreach(L ${LINK_LIBRARIES})
            if (NOT TARGET ${L})
                continue()
            endif()

            GetTargetRuntimeDependenciesRecurse(
                NAME ${L}
                OUTPUT TEMP
            )
            foreach(T ${TEMP})
                list(APPEND RESULT ${T})
            endforeach()
        endforeach()
    endif()

    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(AddRuntimeDependenciesCopyCommand)
    cmake_parse_arguments(PARAMS "" "NAME" "" ${ARGN})

    GetTargetRuntimeDependenciesRecurse(
        NAME ${PARAMS_NAME}
        OUTPUT RUNTIME_DEPS
    )
    foreach(R ${RUNTIME_DEPS})
        add_custom_command(
            TARGET ${PARAMS_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${R} $<TARGET_FILE_DIR:${PARAMS_NAME}>
        )
    endforeach()
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

function(GetTargetIncludeDirectoriesRecurse)
    cmake_parse_arguments(PARAMS "" "NAME;OUTPUT" "" ${ARGN})

    if (NOT (TARGET ${PARAMS_NAME}))
        return()
    endif()

    get_target_property(TARGET_INCS ${PARAMS_NAME} INCLUDE_DIRECTORIES)
    if (NOT ("${TARGET_INCS}" STREQUAL "TARGET_INCS-NOTFOUND"))
        foreach(TARGET_INC ${TARGET_INCS})
            list(APPEND RESULT ${TARGET_INC})
        endforeach()
    endif()

    get_target_property(TARGET_LIBS ${PARAMS_NAME} LINK_LIBRARIES)
    if (NOT ("${TARGET_LIBS}" STREQUAL "TARGET_LIBS-NOTFOUND"))
        foreach(TARGET_LIB ${TARGET_LIBS})
            GetTargetIncludeDirectoriesRecurse(
                NAME ${TARGET_LIB}
                OUTPUT LIB_INCS
            )
            foreach(LIB_INC ${LIB_INCS})
                list(APPEND RESULT ${LIB_INC})
            endforeach()
        endforeach()
    endif()

    list(REMOVE_DUPLICATES RESULT)
    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(AddExecutable)
    cmake_parse_arguments(PARAMS "SAMPLE;META" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES" ${ARGN})

    if (${PARAMS_SAMPLE} AND (NOT ${BUILD_SAMPLE}))
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
    cmake_parse_arguments(PARAMS "META" "NAME;TYPE" "SRC;PRIVATE_INC;PUBLIC_INC;PRIVATE_LINK;LIB" ${ARGN})

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

    cmake_parse_arguments(PARAMS "META" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES" ${ARGN})

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
