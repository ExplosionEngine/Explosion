option(BUILD_TEST "Build unit tests" ON)
option(BUILD_SAMPLE "Build sample" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Api CACHE PATH "" FORCE)
set(META_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Meta CACHE PATH "" FORCE)
set(BASIC_LIBS Common CACHE STRING "" FORCE)
set(BASIC_TEST_LIBS googletest CACHE STRING "" FORCE)

if (${BUILD_TEST})
    enable_testing()
    add_definitions(-DBUILD_TEST=1)
else()
    add_definitions(-DBUILD_TEST=0)
endif()

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

    list(REMOVE_DUPLICATES RESULT)
    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(AddRuntimeDependenciesCopyCommand)
    cmake_parse_arguments(PARAMS "" "NAME" "" ${ARGN})

    GetTargetRuntimeDependenciesRecurse(
        NAME ${PARAMS_NAME}
        OUTPUT RUNTIME_DEPS
    )
    foreach(R ${RUNTIME_DEPS})
        string(FIND ${R} "->" LOCATION)
        if (NOT ${LOCATION} EQUAL -1)
            string(REPLACE "->" ";" TEMP ${R})
            list(GET TEMP 0 SRC)
            list(GET TEMP 1 DST)
            add_custom_command(
                TARGET ${PARAMS_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>/${DST}
            )
        else ()
            add_custom_command(
                TARGET ${PARAMS_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${R} $<TARGET_FILE_DIR:${PARAMS_NAME}>
            )
        endif ()
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

        list(APPEND COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>/${DST})
    endforeach()

    set(COPY_RES_TARGET_NAME ${PARAMS_NAME}.CopyRes)
    add_custom_target(
        ${COPY_RES_TARGET_NAME}
        ${COMMANDS}
    )
    add_dependencies(${PARAMS_NAME} ${COPY_RES_TARGET_NAME})
endfunction()

function(GetTargetIncludeDirectoriesRecurse)
    cmake_parse_arguments(PARAMS "" "NAME;OUTPUT" "" ${ARGN})

    if (NOT (TARGET ${PARAMS_NAME}))
        return()
    endif()

    get_target_property(3RD_TYPE ${PARAMS_NAME} 3RD_TYPE)
    if ("${3RD_TYPE}" STREQUAL "3RD_TYPE-NOTFOUND")
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
    else ()
        get_target_property(3RD_INCLUDE ${PARAMS_NAME} 3RD_INCLUDE)
        if (NOT ("${3RD_INCLUDE}" STREQUAL "3RD_INCLUDE-NOTFOUND"))
            foreach(3RD_INC ${3RD_INCLUDE})
                list(APPEND RESULT ${3RD_INC})
            endforeach()
        endif ()
    endif ()

    list(REMOVE_DUPLICATES RESULT)
    set(${PARAMS_OUTPUT} ${RESULT} PARENT_SCOPE)
endfunction()

function(AddMirrorInfoSourceGenerationTarget)
    cmake_parse_arguments(PARAMS "" "NAME;OUTPUT_SRC;OUTPUT_TARGET_NAME" "SEARCH_DIR;PUBLIC_INC;PRIVATE_INC;LIB" ${ARGN})

    if (DEFINED PARAMS_PUBLIC_INC)
        list(APPEND INC ${PARAMS_PUBLIC_INC})
    endif()
    if (DEFINED PARAMS_PRIVATE_INC)
        list(APPEND INC ${PARAMS_PRIVATE_INC})
    endif()
    if (DEFINED PARAMS_LIB)
        foreach (L ${PARAMS_LIB})
            GetTargetIncludeDirectoriesRecurse(
                NAME ${L}
                OUTPUT TARGET_INCS
            )
            foreach (I ${TARGET_INCS})
                list(APPEND INC ${I})
            endforeach ()
        endforeach()
    endif()
    list(REMOVE_DUPLICATES INC)

    list(APPEND INC_ARGS "-I")
    foreach (I ${INC})
        get_filename_component(ABSOLUTE_I ${I} ABSOLUTE)
        list(APPEND ABSOLUTE_INC ${ABSOLUTE_I})
        list(APPEND INC_ARGS ${ABSOLUTE_I})
    endforeach()

    foreach (SEARCH_DIR ${PARAMS_SEARCH_DIR})
        file(GLOB_RECURSE INPUT_HEADER_FILES "${SEARCH_DIR}/*.h")
        foreach (INPUT_HEADER_FILE ${INPUT_HEADER_FILES})
            get_filename_component(FILENAME ${INPUT_HEADER_FILE} NAME_WE)

            set(OUTPUT_SOURCE "${CMAKE_BINARY_DIR}/Generated/MirrorInfoSource/${PARAMS_NAME}/${FILENAME}.generated.cpp")
            list(APPEND OUTPUT_SOURCES ${OUTPUT_SOURCE})

            add_custom_command(
                OUTPUT ${OUTPUT_SOURCE}
                COMMAND "$<TARGET_FILE:MirrorTool>" "-i" ${INPUT_HEADER_FILE} "-o" ${OUTPUT_SOURCE} ${INC_ARGS}
                DEPENDS MirrorTool ${INPUT_HEADER_FILE}
            )
        endforeach()
    endforeach ()

    set(CUSTOM_TARGET_NAME "${PARAMS_NAME}.Generated")
    add_custom_target(
        ${CUSTOM_TARGET_NAME}
        DEPENDS MirrorTool ${OUTPUT_SOURCES}
    )
    set(${PARAMS_OUTPUT_SRC} ${OUTPUT_SOURCES} PARENT_SCOPE)
    set(${PARAMS_OUTPUT_TARGET_NAME} ${CUSTOM_TARGET_NAME} PARENT_SCOPE)

    if (DEFINED PARAMS_LIB)
        add_dependencies(${CUSTOM_TARGET_NAME} ${PARAMS_LIB})
    endif()
endfunction()

function(AddExecutable)
    cmake_parse_arguments(PARAMS "SAMPLE;META" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES;REFLECT" ${ARGN})

    if (${PARAMS_SAMPLE} AND (NOT ${BUILD_SAMPLE}))
        return()
    endif()

    if (DEFINED PARAMS_REFLECT)
        AddMirrorInfoSourceGenerationTarget(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PRIVATE_INC ${PARAMS_INC}
            LIB ${PARAMS_LIB}
        )
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
        ${GENERATED_SRC}
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
    if (DEFINED PARAMS_REFLECT)
        add_dependencies(${PARAMS_NAME} ${GENERATED_TARGET})
    endif()

    if (${MSVC})
        set_target_properties(${PARAMS_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
    endif()
endfunction()

function(AddLibrary)
    cmake_parse_arguments(PARAMS "META" "NAME;TYPE" "SRC;PRIVATE_INC;PUBLIC_INC;PRIVATE_LINK;LIB;REFLECT" ${ARGN})

    if ("${PARAMS_TYPE}" STREQUAL "SHARED")
        list(APPEND PARAMS_PUBLIC_INC ${API_HEADER_DIR}/${PARAMS_NAME})
    endif ()

    if (DEFINED PARAMS_REFLECT)
        AddMirrorInfoSourceGenerationTarget(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PUBLIC_INC ${PARAMS_PUBLIC_INC}
            PRIVATE_INC ${PARAMS_PRIVATE_INC}
            LIB ${PARAMS_LIB}
        )
    endif()

    add_library(
        ${PARAMS_NAME}
        ${PARAMS_TYPE}
        ${PARAMS_SRC}
        ${GENERATED_SRC}
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
            PUBLIC /MD$<$<CONFIG:Debug>:d>
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
    endif()

    if (DEFINED PARAMS_REFLECT)
        add_dependencies(${PARAMS_NAME} ${GENERATED_TARGET})
    endif()
endfunction()

function(AddTest)
    if (NOT ${BUILD_TEST})
        return()
    endif()

    cmake_parse_arguments(PARAMS "META" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES;REFLECT" ${ARGN})

    if (DEFINED PARAMS_REFLECT)
        AddMirrorInfoSourceGenerationTarget(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PRIVATE_INC ${PARAMS_INC}
            LIB ${PARAMS_LIB} ${BASIC_LIBS} ${BASIC_TEST_LIBS}
        )
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
        ${GENERATED_SRC}
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
    if (DEFINED PARAMS_REFLECT)
        add_dependencies(${PARAMS_NAME} ${GENERATED_TARGET})
    endif()

    add_test(
        NAME ${PARAMS_NAME}
        COMMAND ${PARAMS_NAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${PARAMS_NAME}>
    )
endfunction()
