option(BUILD_TEST "Build unit tests" ON)
option(BUILD_SAMPLE "Build sample" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Api CACHE PATH "" FORCE)
set(BASIC_LIBS Common CACHE STRING "" FORCE)
set(BASIC_TEST_LIBS Test CACHE STRING "" FORCE)

if (${BUILD_TEST})
    enable_testing()
    add_definitions(-DBUILD_TEST=1)
else()
    add_definitions(-DBUILD_TEST=0)
endif()

function(exp_combine_runtime_deps)
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

function(exp_link_libs)
    cmake_parse_arguments(PARAMS "" "NAME" "LIB" ${ARGN})

    foreach(L ${PARAMS_LIB})
        if (NOT (TARGET ${L}))
            target_link_libraries(${PARAMS_NAME} PUBLIC ${L})
            continue()
        endif()

        get_target_property(3RD_TYPE ${L} 3RD_TYPE)
        if (${3RD_TYPE} STREQUAL "3RD_TYPE-NOTFOUND")
            target_link_libraries(${PARAMS_NAME} PUBLIC ${L})
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
                target_link_libraries(${PARAMS_NAME} PUBLIC ${LIB})
            endif()

            if (${3RD_TYPE} STREQUAL "CMakeProject")
                add_dependencies(${PARAMS_NAME} ${L})
            endif()
        endif()

        get_target_property(RUNTIME_DEP ${L} 3RD_RUNTIME_DEP)
        if (NOT ("${RUNTIME_DEP}" STREQUAL "RUNTIME_DEP-NOTFOUND"))
            exp_combine_runtime_deps(
                NAME ${PARAMS_NAME}
                RUNTIME_DEP "${RUNTIME_DEP}"
            )
        endif()
    endforeach()
endfunction()

function(exp_link_basic_libs)
    cmake_parse_arguments(PARAMS "" "NAME" "LIB" ${ARGN})

    foreach(L ${PARAMS_LIB})
        if (NOT (${PARAMS_NAME} STREQUAL ${L}))
            exp_link_libs(
                NAME ${PARAMS_NAME}
                LIB ${L}
            )
        endif()
    endforeach()
endfunction()

function(exp_get_target_runtime_deps_recurse)
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

            exp_get_target_runtime_deps_recurse(
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

function(exp_add_runtime_deps_copy_command)
    cmake_parse_arguments(PARAMS "NOT_INSTALL" "NAME" "" ${ARGN})

    exp_get_target_runtime_deps_recurse(
        NAME ${PARAMS_NAME}
        OUTPUT RUNTIME_DEPS
    )
    foreach(R ${RUNTIME_DEPS})
        string(FIND ${R} "->" LOCATION)
        if (NOT ${LOCATION} EQUAL -1)
            string(REPLACE "->" ";" TEMP ${R})
            list(GET TEMP 0 SRC)
            list(GET TEMP 1 DST)
            set(COPY_COMMAND ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>/${DST})
            set(INSTALL_DST ${CMAKE_INSTALL_PREFIX}/Engine/Binaries/${DST})
        else ()
            set(SRC ${R})
            set(COPY_COMMAND ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>)
            set(INSTALL_DST ${CMAKE_INSTALL_PREFIX}/Engine/Binaries)
        endif ()

        if (IS_DIRECTORY ${SRC})
            add_custom_command(
                TARGET ${PARAMS_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${COPY_COMMAND}
            )
            if (NOT ${PARAMS_NOT_INSTALL})
                install(
                    DIRECTORY ${SRC} DESTINATION ${INSTALL_DST}
                )
            endif ()
        else ()
            add_custom_command(
                TARGET ${PARAMS_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${COPY_COMMAND}
            )
            if (NOT ${PARAMS_NOT_INSTALL})
                install(
                    FILES ${SRC} DESTINATION ${INSTALL_DST}
                )
            endif ()
        endif ()
    endforeach()
endfunction()

function(exp_expand_resource_path_expression)
    cmake_parse_arguments(PARAMS "" "INPUT;OUTPUT_SRC;OUTPUT_DST" "" ${ARGN})

    string(REPLACE "->" ";" TEMP ${PARAMS_INPUT})
    list(GET TEMP 0 SRC)
    list(GET TEMP 1 DST)

    set(${PARAMS_OUTPUT_SRC} ${SRC} PARENT_SCOPE)
    set(${PARAMS_OUTPUT_DST} ${DST} PARENT_SCOPE)
endfunction()

function(exp_add_resources_copy_command)
    cmake_parse_arguments(PARAMS "NOT_INSTALL" "NAME" "RES" ${ARGN})

    foreach(R ${PARAMS_RES})
        exp_expand_resource_path_expression(
            INPUT ${R}
            OUTPUT_SRC SRC
            OUTPUT_DST DST
        )

        list(APPEND COPY_COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC} $<TARGET_FILE_DIR:${PARAMS_NAME}>/${DST})

        get_filename_component(ABSOLUTE_DST ${CMAKE_INSTALL_PREFIX}/Engine/Binaries/${DST} ABSOLUTE)
        get_filename_component(DST_DIR ${ABSOLUTE_DST} DIRECTORY)
        if (NOT ${PARAMS_NOT_INSTALL})
            install(FILES ${SRC} DESTINATION ${DST_DIR})
        endif ()
    endforeach()

    set(COPY_RES_TARGET_NAME ${PARAMS_NAME}.CopyRes)
    add_custom_target(
        ${COPY_RES_TARGET_NAME}
        ${COPY_COMMANDS}
    )
    add_dependencies(${PARAMS_NAME} ${COPY_RES_TARGET_NAME})
endfunction()

function(exp_get_target_include_dirs_recurse)
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
                exp_get_target_include_dirs_recurse(
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

function(exp_add_mirror_info_source_generation_target)
    cmake_parse_arguments(PARAMS "DYNAMIC" "NAME;OUTPUT_SRC;OUTPUT_TARGET_NAME" "SEARCH_DIR;PUBLIC_INC;PRIVATE_INC;LIB" ${ARGN})

    if (DEFINED PARAMS_PUBLIC_INC)
        list(APPEND INC ${PARAMS_PUBLIC_INC})
    endif()
    if (DEFINED PARAMS_PRIVATE_INC)
        list(APPEND INC ${PARAMS_PRIVATE_INC})
    endif()
    if (DEFINED PARAMS_LIB)
        foreach (L ${PARAMS_LIB})
            exp_get_target_include_dirs_recurse(
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

    if (${PARAMS_DYNAMIC})
        list(APPEND DYNAMIC_ARG "-d")
    endif ()

    foreach (SEARCH_DIR ${PARAMS_SEARCH_DIR})
        file(GLOB_RECURSE INPUT_HEADER_FILES "${SEARCH_DIR}/*.h")
        foreach (INPUT_HEADER_FILE ${INPUT_HEADER_FILES})
            string(REPLACE "${CMAKE_SOURCE_DIR}/" "" TEMP ${INPUT_HEADER_FILE})
            get_filename_component(DIR ${TEMP} DIRECTORY)
            get_filename_component(FILENAME ${TEMP} NAME_WE)

            set(OUTPUT_SOURCE "${CMAKE_BINARY_DIR}/Generated/MirrorInfoSource/${DIR}/${FILENAME}.generated.cpp")
            list(APPEND OUTPUT_SOURCES ${OUTPUT_SOURCE})

            add_custom_command(
                OUTPUT ${OUTPUT_SOURCE}
                COMMAND "$<TARGET_FILE:MirrorTool>" ${DYNAMIC_ARG} "-i" ${INPUT_HEADER_FILE} "-o" ${OUTPUT_SOURCE} ${INC_ARGS}
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

function(exp_add_executable)
    cmake_parse_arguments(PARAMS "SAMPLE;NOT_INSTALL" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES;REFLECT" ${ARGN})

    if (${PARAMS_SAMPLE} AND (NOT ${BUILD_SAMPLE}))
        return()
    endif()

    if (${PARAMS_NOT_INSTALL})
        set(NOT_INSTALL_FLAG NOT_INSTALL)
    else ()
        set(NOT_INSTALL_FLAG "")
    endif ()

    if (DEFINED PARAMS_REFLECT)
        exp_add_mirror_info_source_generation_target(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PRIVATE_INC ${PARAMS_INC}
            LIB ${PARAMS_LIB}
        )
    endif()

    add_executable(${PARAMS_NAME})
    target_sources(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_SRC} ${GENERATED_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_LINK}
    )
    exp_link_basic_libs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS}
    )
    exp_link_libs(
        NAME ${PARAMS_NAME}
        LIB ${PARAMS_LIB}
    )
    exp_add_runtime_deps_copy_command(
        NAME ${PARAMS_NAME}
        ${NOT_INSTALL_FLAG}
    )
    exp_add_resources_copy_command(
        NAME ${PARAMS_NAME}
        RES ${PARAMS_RES}
        ${NOT_INSTALL_FLAG}
    )
    if (DEFINED PARAMS_DEP_TARGET)
        add_dependencies(${PARAMS_NAME} ${PARAMS_DEP_TARGET})
    endif()
    if (DEFINED PARAMS_REFLECT)
        add_dependencies(${PARAMS_NAME} ${GENERATED_TARGET})
    endif()

    if (${MSVC})
        set_target_properties(${PARAMS_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()

    if (NOT ${PARAMS_NOT_INSTALL})
        install(
            TARGETS ${PARAMS_NAME}
            RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Binaries
        )

        if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            install(CODE "execute_process(COMMAND install_name_tool -add_rpath @executable_path ${CMAKE_INSTALL_PREFIX}/Engine/Binaries/$<TARGET_FILE_NAME:${PARAMS_NAME}>)")
        endif ()
    endif ()
endfunction()

function(exp_add_library)
    cmake_parse_arguments(PARAMS "NOT_INSTALL" "NAME;TYPE" "SRC;PRIVATE_INC;PUBLIC_INC;PRIVATE_LINK;LIB;REFLECT" ${ARGN})

    if ("${PARAMS_TYPE}" STREQUAL "SHARED")
        list(APPEND PARAMS_PUBLIC_INC ${API_HEADER_DIR}/${PARAMS_NAME})
    endif ()

    if (DEFINED PARAMS_REFLECT)
        if ("${PARAMS_TYPE}" STREQUAL "SHARED")
            list(APPEND EXTRA_PARAMS DYNAMIC)
        endif ()

        exp_add_mirror_info_source_generation_target(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PUBLIC_INC ${PARAMS_PUBLIC_INC}
            PRIVATE_INC ${PARAMS_PRIVATE_INC}
            LIB ${PARAMS_LIB}
            ${EXTRA_PARAMS}
        )
    endif()

    add_library(
        ${PARAMS_NAME}
        ${PARAMS_TYPE}
    )
    target_sources(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_SRC} ${GENERATED_SRC}
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
    exp_link_basic_libs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS}
    )
    exp_link_libs(
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

    if (NOT ${PARAMS_NOT_INSTALL})
        foreach (INC ${PARAMS_PUBLIC_INC})
            get_filename_component(ABSOLUTE_INC ${INC} ABSOLUTE)
            file(GLOB_RECURSE PUBLIC_HEADERS ${ABSOLUTE_INC}/*.h)
            target_sources(
                ${PARAMS_NAME}
                PUBLIC FILE_SET HEADERS
                BASE_DIRS ${ABSOLUTE_INC} FILES ${PUBLIC_HEADERS}
            )
        endforeach ()

        if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" OR "${PARAMS_TYPE}" STREQUAL "STATIC")
            install(
                TARGETS ${PARAMS_NAME}
                FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Include
                ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Lib
                LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Lib
                RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Binaries
            )
        endif ()

        if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" AND "${PARAMS_TYPE}" STREQUAL "SHARED")
            install(
                FILES $<TARGET_FILE:${PARAMS_NAME}>
                DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Binaries
            )
        endif ()
    endif ()
endfunction()

function(exp_add_test)
    if (NOT ${BUILD_TEST})
        return()
    endif()

    cmake_parse_arguments(PARAMS "META" "NAME" "SRC;INC;LINK;LIB;DEP_TARGET;RES;REFLECT" ${ARGN})

    if (DEFINED PARAMS_REFLECT)
        exp_add_mirror_info_source_generation_target(
            NAME ${PARAMS_NAME}
            OUTPUT_SRC GENERATED_SRC
            OUTPUT_TARGET_NAME GENERATED_TARGET
            SEARCH_DIR ${PARAMS_REFLECT}
            PRIVATE_INC ${PARAMS_INC}
            LIB ${PARAMS_LIB} ${BASIC_LIBS} ${BASIC_TEST_LIBS}
        )
    endif()

    add_executable(${PARAMS_NAME})
    target_sources(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_SRC} ${GENERATED_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_LINK}
    )
    exp_link_basic_libs(
        NAME ${PARAMS_NAME}
        LIB ${BASIC_LIBS} ${BASIC_TEST_LIBS}
    )
    exp_link_libs(
        NAME ${PARAMS_NAME}
        LIB ${PARAMS_LIB}
    )
    exp_add_runtime_deps_copy_command(
        NAME ${PARAMS_NAME}
        NOT_INSTALL
    )
    exp_add_resources_copy_command(
        NAME ${PARAMS_NAME}
        RES ${PARAMS_RES}
        NOT_INSTALL
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
