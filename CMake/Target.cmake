option(BUILD_TEST "Build unit tests" ON)
option(BUILD_SAMPLE "Build sample" ON)

set(API_HEADER_DIR ${CMAKE_BINARY_DIR}/Generated/Api CACHE PATH "" FORCE)

if (${BUILD_TEST})
    enable_testing()
    add_definitions(-DBUILD_TEST=1)
else()
    add_definitions(-DBUILD_TEST=0)
endif()

function(exp_gather_target_runtime_dependencies_recurse)
    set(options "")
    set(singleValueArgs NAME OUT_RUNTIME_DEP)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET ${arg_NAME})
        set(${arg_OUT_RUNTIME_DEP} "" PARENT_SCOPE)
        return()
    endif ()

    get_target_property(runtime_dep ${arg_NAME} RUNTIME_DEP)
    if (NOT ("${runtime_dep}" STREQUAL "runtime_dep-NOTFOUND"))
        foreach(r ${runtime_dep})
            # workaround to make EXPORT_PROPERTIES support generator expression
            string(REPLACE "[" "$<" r "${r}")
            string(REPLACE "]" ">" r "${r}")

            get_target_property(type ${arg_NAME} TYPE)
            if (${type} STREQUAL "SHARED_LIBRARY")
                set(target_bin_dir $<TARGET_FILE_DIR:${arg_NAME}>)
            else ()
                set(target_bin_dir $<TARGET_FILE_DIR:${arg_NAME}>/../Binaries)
            endif ()
            string(REPLACE "$<TARGET_BIN_DIR>" ${target_bin_dir} temp_r ${r})
            list(APPEND result_runtime_dep ${temp_r})
        endforeach()
    endif()

    get_target_property(libs ${arg_NAME} LINK_LIBRARIES)
    if (NOT ("${libs}" STREQUAL "libs-NOTFOUND"))
        foreach(l ${libs})
            if (NOT TARGET ${l})
                continue()
            endif()

            get_target_property(type ${l} TYPE)
            if (${type} STREQUAL SHARED_LIBRARY)
                list(APPEND result_runtime_dep $<TARGET_FILE:${l}>)
            endif ()

            exp_gather_target_runtime_dependencies_recurse(
                NAME ${l}
                OUT_RUNTIME_DEP temp_runtime_dep
            )
            foreach(t ${temp_runtime_dep})
                list(APPEND result_runtime_dep ${t})
            endforeach()
        endforeach()
    endif()

    list(REMOVE_DUPLICATES result_runtime_dep)
    set(${arg_OUT_RUNTIME_DEP} ${result_runtime_dep} PARENT_SCOPE)
endfunction()

function(exp_process_runtime_dependencies)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME)
    set(multiValueArgs DEP_TARGET)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    exp_gather_target_runtime_dependencies_recurse(
        NAME ${arg_NAME}
        OUT_RUNTIME_DEP runtime_deps
    )
    foreach (d ${arg_DEP_TARGET})
        list(APPEND runtime_deps $<TARGET_FILE:${d}>)
    endforeach ()
    foreach(r ${runtime_deps})
        add_custom_command(
            TARGET ${arg_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${r} $<TARGET_FILE_DIR:${arg_NAME}>
        )
        if (NOT ${arg_NOT_INSTALL})
            install(
                FILES ${r} DESTINATION ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries
            )
        endif ()
    endforeach()
endfunction()

function(exp_expand_resource_path_expression)
    set(options "")
    set(singleValueArgs INPUT OUTPUT_SRC OUTPUT_DST)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    string(REPLACE "->" ";" temp ${arg_INPUT})
    list(GET temp 0 src)
    list(GET temp 1 dst)

    set(${arg_OUTPUT_SRC} ${src} PARENT_SCOPE)
    set(${arg_OUTPUT_DST} ${dst} PARENT_SCOPE)
endfunction()

function(exp_add_resources_copy_command)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME)
    set(multiValueArgs RES)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(r ${arg_RES})
        exp_expand_resource_path_expression(
            INPUT ${r}
            OUTPUT_SRC src
            OUTPUT_DST dst
        )

        list(APPEND copy_commands COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} $<TARGET_FILE_DIR:${arg_NAME}>/${dst})

        get_filename_component(absolute_dst ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/${dst} ABSOLUTE)
        get_filename_component(dst_dir ${absolute_dst} DIRECTORY)
        if (NOT ${arg_NOT_INSTALL})
            install(FILES ${src} DESTINATION ${dst_dir})
        endif ()
    endforeach()

    set(copy_res_target_name ${arg_NAME}.CopyRes)
    add_custom_target(
        ${copy_res_target_name}
        ${copy_commands}
    )
    add_dependencies(${arg_NAME} ${copy_res_target_name})
endfunction()

function(exp_gather_target_libs)
    set(options "")
    set(singleValueArgs NAME OUTPUT)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    string(REGEX MATCH "\\$\\<BUILD_INTERFACE.+\\>" match ${arg_NAME})
    if (match)
        set(${arg_OUTPUT} "" PARENT_SCOPE)
    endif ()

    get_target_property(target_libs ${arg_NAME} LINK_LIBRARIES)
    if (NOT ("${target_libs}" STREQUAL "target_libs-NOTFOUND"))
        foreach(target_lib ${target_libs})
            string(REGEX MATCH "\\$\\<BUILD_INTERFACE.+\\>" match ${target_lib})
            if (match)
                continue()
            endif ()

            list(APPEND result ${target_lib})
            exp_gather_target_libs(
                NAME ${target_lib}
                OUTPUT libs
            )
            foreach(lib ${libs})
                list(APPEND result ${lib})
            endforeach()
        endforeach()
    endif()

    list(REMOVE_DUPLICATES result)
    set(${arg_OUTPUT} ${result} PARENT_SCOPE)
endfunction()

function(exp_add_mirror_info_source_generation_target)
    set(options DYNAMIC)
    set(singleValueArgs NAME OUTPUT_SRC OUTPUT_TARGET_NAME)
    set(multiValueArgs SEARCH_DIR PUBLIC_INC PRIVATE_INC LIB FRAMEWORK_DIR)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (DEFINED arg_PUBLIC_INC)
        foreach (i ${arg_PUBLIC_INC})
            get_filename_component(absolute_i ${i} ABSOLUTE)
            list(APPEND inc ${absolute_i})
        endforeach ()
    endif()
    if (DEFINED arg_PRIVATE_INC)
        foreach (i ${arg_PRIVATE_INC})
            get_filename_component(absolute_i ${i} ABSOLUTE)
            list(APPEND inc ${absolute_i})
        endforeach ()
    endif()
    if (DEFINED arg_LIB)
        foreach (l ${arg_LIB})
            list(APPEND inc $<TARGET_PROPERTY:${l},INTERFACE_INCLUDE_DIRECTORIES>)
            exp_gather_target_libs(
                NAME ${l}
                OUTPUT target_libs
            )
            foreach (tl ${target_libs})
                list(APPEND inc $<TARGET_PROPERTY:${tl},INTERFACE_INCLUDE_DIRECTORIES>)
            endforeach ()
        endforeach()
    endif()
    list(REMOVE_DUPLICATES inc)

    list(APPEND inc_args "-I")
    foreach (i ${inc})
        list(APPEND inc_args ${i})
    endforeach()

    if (DEFINED arg_FRAMEWORK_DIR)
        list(APPEND fwk_dir ${arg_FRAMEWORK_DIR})
        list(APPEND fwk_dir_args "-F")
    endif ()
    list(REMOVE_DUPLICATES fwk_dir)

    foreach (f ${fwk_dir})
        get_filename_component(absolute_f ${f} ABSOLUTE)
        list(APPEND fwk_dir_args ${absolute_f})
    endforeach ()

    if (${arg_DYNAMIC})
        list(APPEND dynamic_arg "-d")
    endif ()

    foreach (search_dir ${arg_SEARCH_DIR})
        file(GLOB_RECURSE input_header_files "${search_dir}/*.h")
        foreach (input_header_file ${input_header_files})
            string(REPLACE "${CMAKE_SOURCE_DIR}/" "" temp ${input_header_file})
            get_filename_component(dir ${temp} DIRECTORY)
            get_filename_component(filename ${temp} NAME_WE)

            set(output_source "${CMAKE_BINARY_DIR}/Generated/MirrorInfoSource/${dir}/${filename}.generated.cpp")
            list(APPEND output_sources ${output_source})

            add_custom_command(
                OUTPUT ${output_source}
                COMMAND "$<TARGET_FILE:MirrorTool>" ${dynamic_arg} "-i" ${input_header_file} "-o" ${output_source} ${inc_args} ${fwk_dir_args}
                DEPENDS MirrorTool ${input_header_file}
            )
        endforeach()
    endforeach ()

    set(custom_target_name "${arg_NAME}.Generated")
    add_custom_target(
        ${custom_target_name}
        DEPENDS MirrorTool ${output_sources}
    )
    set(${arg_OUTPUT_SRC} ${output_sources} PARENT_SCOPE)
    set(${arg_OUTPUT_TARGET_NAME} ${custom_target_name} PARENT_SCOPE)

    if (DEFINED arg_LIB)
        add_dependencies(${custom_target_name} ${arg_LIB})
    endif()
endfunction()

function(exp_add_executable)
    set(options SAMPLE NOT_INSTALL)
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC LINK LIB DEP_TARGET RES REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (${arg_SAMPLE} AND (NOT ${BUILD_SAMPLE}))
        return()
    endif()

    if (${arg_NOT_INSTALL})
        set(not_install_flag NOT_INSTALL)
    else ()
        set(not_install_flag "")
    endif ()

    if (DEFINED arg_REFLECT)
        exp_add_mirror_info_source_generation_target(
            NAME ${arg_NAME}
            OUTPUT_SRC generated_src
            OUTPUT_TARGET_NAME generated_target
            SEARCH_DIR ${arg_REFLECT}
            PRIVATE_INC ${arg_INC}
            LIB ${arg_LIB}
        )
    endif()

    add_executable(${arg_NAME})
    target_sources(
        ${arg_NAME}
        PRIVATE ${arg_SRC} ${generated_src}
    )
    get_cmake_property(generated_is_multi_config GENERATOR_IS_MULTI_CONFIG)
    if (${generated_is_multi_config})
        set(runtime_output_dir ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/${SUB_PROJECT_NAME}/Binaries)
    else ()
        set(runtime_output_dir ${CMAKE_BINARY_DIR}/Dist/${SUB_PROJECT_NAME}/Binaries)
    endif ()
    set_target_properties(
        ${arg_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${runtime_output_dir}
    )

    target_include_directories(
        ${arg_NAME}
        PRIVATE ${arg_INC}
    )
    target_link_directories(
        ${arg_NAME}
        PRIVATE ${arg_LINK}
    )
    target_link_libraries(
        ${arg_NAME}
        PUBLIC ${arg_LIB}
    )
    exp_process_runtime_dependencies(
        NAME ${arg_NAME}
        DEP_TARGET ${arg_DEP_TARGET}
        ${not_install_flag}
    )
    exp_add_resources_copy_command(
        NAME ${arg_NAME}
        RES ${arg_RES}
        ${not_install_flag}
    )
    if (DEFINED arg_DEP_TARGET)
        add_dependencies(${arg_NAME} ${arg_DEP_TARGET})
    endif()
    if (DEFINED arg_REFLECT)
        add_dependencies(${arg_NAME} ${generated_target})
    endif()

    if (${MSVC})
        set_target_properties(
            ${arg_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY ${runtime_output_dir}
        )
    endif()

    if (NOT ${arg_NOT_INSTALL})
        install(
            TARGETS ${arg_NAME}
            EXPORT ${SUB_PROJECT_NAME}Targets
            RUNTIME DESTINATION ${SUB_PROJECT_NAME}/Binaries
        )
        export(
            TARGETS ${arg_NAME}
            NAMESPACE ${SUB_PROJECT_NAME}::
            APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
        )

        if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
            install(CODE "execute_process(COMMAND install_name_tool -add_rpath @executable_path ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/$<TARGET_FILE_NAME:${arg_NAME}>)")
        endif ()
    endif ()
endfunction()

function(exp_add_library)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME TYPE)
    set(multiValueArgs SRC PRIVATE_INC PUBLIC_INC PRIVATE_LINK PUBLIC_LINK PRIVATE_LIB PUBLIC_LIB PRIVATE_MERGE_LIB PUBLIC_MERGE_LIB REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${arg_TYPE}" STREQUAL "SHARED")
        list(APPEND arg_PUBLIC_INC ${API_HEADER_DIR}/${arg_NAME})
    endif ()

    if (DEFINED arg_REFLECT)
        if ("${arg_TYPE}" STREQUAL "SHARED")
            list(APPEND extra_params DYNAMIC)
        endif ()

        exp_add_mirror_info_source_generation_target(
            NAME ${arg_NAME}
            OUTPUT_SRC generated_src
            OUTPUT_TARGET_NAME generated_target
            SEARCH_DIR ${arg_REFLECT}
            PUBLIC_INC ${arg_PUBLIC_INC}
            PRIVATE_INC ${arg_PRIVATE_INC}
            LIB ${arg_PRIVATE_LIB} ${arg_PUBLIC_LIB}
            ${extra_params}
        )
    endif()

    add_library(
        ${arg_NAME}
        ${arg_TYPE}
    )
    target_sources(
        ${arg_NAME}
        PRIVATE ${arg_SRC} ${generated_src}
    )
    get_cmake_property(generator_is_multi_config GENERATOR_IS_MULTI_CONFIG)
    if (${generator_is_multi_config})
        set(runtime_output_dir ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/$<CONFIG>/Binaries)
        set(library_output_dir ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/$<CONFIG>/Binaries)
        set(archive_output_directory ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/$<CONFIG>/Lib)
    else ()
        set(runtime_output_dir ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/Binaries)
        set(library_output_dir ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/Binaries)
        set(archive_output_directory ${CMAKE_BINARY_DIR}/Targets/${SUB_PROJECT_NAME}/${arg_NAME}/Lib)
    endif ()
    set_target_properties(
        ${arg_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${runtime_output_dir}
        LIBRARY_OUTPUT_DIRECTORY ${library_output_dir}
        ARCHIVE_OUTPUT_DIRECTORY ${archive_output_directory}
    )

    foreach (inc ${arg_PUBLIC_INC})
        list(APPEND public_inc ${inc})
    endforeach ()
    foreach (lib ${arg_PUBLIC_MERGE_LIB})
        if (TARGET ${lib})
            list(APPEND public_inc $<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>)
        endif ()
    endforeach ()

    foreach (inc ${public_inc})
        string(REGEX MATCH "\\$\\<.+\\>" match ${inc})
        if (match)
            list(APPEND public_build_inc $<BUILD_INTERFACE:${inc}>)
        else ()
            get_filename_component(absolute_inc ${inc} ABSOLUTE)
            list(APPEND public_build_inc $<BUILD_INTERFACE:${absolute_inc}>)
        endif ()
    endforeach ()
    target_include_directories(
        ${arg_NAME}
        PRIVATE ${arg_PRIVATE_INC}
        PUBLIC ${public_build_inc} $<INSTALL_INTERFACE:${SUB_PROJECT_NAME}/Target/${arg_NAME}/Include>
    )

    target_link_directories(
        ${arg_NAME}
        PRIVATE ${arg_PRIVATE_LINK}
        PUBLIC ${arg_PUBLIC_LINK}
    )

    foreach (lib ${arg_PRIVATE_MERGE_LIB})
        list(APPEND merge_lib ${lib})
    endforeach ()
    foreach (lib ${arg_PUBLIC_MERGE_LIB})
        list(APPEND merge_lib ${lib})
    endforeach ()

    foreach (lib ${merge_lib})
        list(APPEND build_merge_lib $<BUILD_INTERFACE:${lib}>)

        exp_gather_target_runtime_dependencies_recurse(
            NAME ${lib}
            OUT_RUNTIME_DEP temp_runtime_dep
        )
        list(APPEND runtime_dep ${temp_runtime_dep})
    endforeach ()
    target_link_libraries(
        ${arg_NAME}
        PRIVATE ${build_merge_lib}
        PUBLIC ${arg_PUBLIC_LIB}
    )

    if (DEFINED runtime_dep)
        foreach (r ${runtime_dep})
            get_filename_component(FILE_NAME ${r} NAME)
            list(APPEND commands COMMAND ${CMAKE_COMMAND} -E copy_if_different ${r} ${runtime_output_dir}/${FILE_NAME})
            list(APPEND runtime_dep_files $<TARGET_BIN_DIR>/${FILE_NAME})
        endforeach ()
        add_custom_command(
            TARGET ${arg_NAME} POST_BUILD
            ${commands}
        )

        # workaround to make EXPORT_PROPERTIES support generator expression
        string(REPLACE "$<" "[" runtime_dep_files "${runtime_dep_files}")
        string(REPLACE ">" "]" runtime_dep_files "${runtime_dep_files}")
        set_target_properties(
            ${arg_NAME} PROPERTIES
            EXPORT_PROPERTIES "runtime_dep"
            RUNTIME_DEP "${runtime_dep_files}"
        )
    endif ()

    if ("${arg_TYPE}" STREQUAL "SHARED")
        string(TOUPPER ${arg_NAME}_API api_name)
        string(REPLACE "-" "/" api_dir ${arg_NAME})

        generate_export_header(
            ${arg_NAME}
            EXPORT_MACRO_NAME ${api_name}
            EXPORT_FILE_NAME ${API_HEADER_DIR}/${arg_NAME}/${api_dir}/Api.h
        )
    endif()

    if (DEFINED arg_REFLECT)
        add_dependencies(${arg_NAME} ${generated_target})
    endif()

    if (NOT ${arg_NOT_INSTALL})
        foreach (inc ${public_inc})
            list(APPEND install_inc ${inc}/)
        endforeach ()
        install(
            DIRECTORY ${install_inc}
            DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Include
        )

        if (DEFINED runtime_dep)
            install(
                FILES ${runtime_dep}
                DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Binaries
            )
        endif ()

        install(
            TARGETS ${arg_NAME}
            EXPORT ${SUB_PROJECT_NAME}Targets
            ARCHIVE DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Lib
            LIBRARY DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Lib
            RUNTIME DESTINATION ${SUB_PROJECT_NAME}/Binaries
        )
        export(
            TARGETS ${arg_NAME}
            NAMESPACE ${SUB_PROJECT_NAME}::
            APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
        )
        if ("${arg_TYPE}" STREQUAL "SHARED")
            install(
                FILES $<TARGET_FILE:${arg_NAME}>
                DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Binaries
            )
        endif ()
    endif ()
endfunction()

function(exp_add_test)
    if (NOT ${BUILD_TEST})
        return()
    endif()

    set(options META)
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC LINK LIB DEP_TARGET RES REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (DEFINED arg_REFLECT)
        exp_add_mirror_info_source_generation_target(
            NAME ${arg_NAME}
            OUTPUT_SRC generated_src
            OUTPUT_TARGET_NAME generated_target
            SEARCH_DIR ${arg_REFLECT}
            PRIVATE_INC ${arg_INC}
            LIB ${arg_LIB}
        )
    endif()

    add_executable(${arg_NAME})
    target_sources(
        ${arg_NAME}
        PRIVATE ${arg_SRC} ${generated_src}
    )
    get_cmake_property(generator_is_multi_config GENERATOR_IS_MULTI_CONFIG)
    if (${generator_is_multi_config})
        set_target_properties(
            ${arg_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/${SUB_PROJECT_NAME}/Binaries
        )
    else ()
        set_target_properties(
            ${arg_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/${SUB_PROJECT_NAME}/Binaries
        )
    endif ()

    target_include_directories(
        ${arg_NAME}
        PRIVATE ${arg_INC}
    )
    target_link_directories(
        ${arg_NAME}
        PRIVATE ${arg_LINK}
    )
    target_link_libraries(
        ${arg_NAME}
        PRIVATE Test ${arg_LIB}
    )
    exp_process_runtime_dependencies(
        NAME ${arg_NAME}
        DEP_TARGET ${arg_DEP_TARGET}
        NOT_INSTALL
    )
    exp_add_resources_copy_command(
        NAME ${arg_NAME}
        RES ${arg_RES}
        NOT_INSTALL
    )
    if (DEFINED arg_DEP_TARGET)
        add_dependencies(${arg_NAME} ${arg_DEP_TARGET})
    endif()
    if (DEFINED arg_REFLECT)
        add_dependencies(${arg_NAME} ${generated_target})
    endif()

    add_test(
        NAME ${arg_NAME}
        COMMAND ${arg_NAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${arg_NAME}>
    )
endfunction()

install(
    EXPORT ${SUB_PROJECT_NAME}Targets
    FILE ${SUB_PROJECT_NAME}Targets.cmake
    NAMESPACE ${SUB_PROJECT_NAME}::
    DESTINATION ${SUB_PROJECT_NAME}/CMake
)
