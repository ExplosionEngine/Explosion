include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

option(BUILD_TEST "Build unit tests" ON)
option(BUILD_BENCHMARK "Build benchmarks" ON)

set(GENERATED_DIR ${CMAKE_BINARY_DIR}/Generated)
set(GENERATED_API_HEADER_DIR ${GENERATED_DIR}/Api)
set(GENERATED_MIRROR_INFO_SRC_DIR ${GENERATED_DIR}/MirrorInfoSrc)
set(BASE_TARGETS_FOLDER "${SUB_PROJECT_NAME}")
set(AUX_TARGETS_FOLDER "${BASE_TARGETS_FOLDER}/Aux")

get_cmake_property(with_multi_config_generator GENERATOR_IS_MULTI_CONFIG)

if (BUILD_TEST)
    enable_testing()
    add_compile_definitions(BUILD_TEST=1)
else()
    add_compile_definitions(BUILD_TEST=0)
endif()

if (BUILD_BENCHMARK)
    add_compile_definitions(BUILD_BENCHMARK=1)
else()
    add_compile_definitions(BUILD_BENCHMARK=0)
endif()

if ("${SUB_PROJECT_NAME}" STREQUAL "")
    message(FATAL_ERROR "SUB_PROJECT_NAME not defined, please set it in your project cmake")
endif ()

# The engine build includes this file directly and exports its targets so downstream projects can find_package() it.
# Those downstream projects pull these helpers back in transitively while their package config runs (CMake sets
# CMAKE_FIND_PACKAGE_NAME during that include), and they are terminal consumers - games or plugins nobody find_package()s
# - so they only ship their runtime binaries, never their own package/export files. Detecting that here keeps the export
# machinery out of downstream install trees without any per-project switch.
if (CMAKE_FIND_PACKAGE_NAME)
    set(export_sub_project_package OFF)
else ()
    set(export_sub_project_package ON)
endif ()

function(exp_gather_target_runtime_dependencies_recurse)
    set(options "")
    set(singleValueArgs NAME OUT_RUNTIME_DEP OUT_DEP_TARGET)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET ${arg_NAME})
        set(${arg_OUT_RUNTIME_DEP} "" PARENT_SCOPE)
        set(${arg_OUT_DEP_TARGET} "" PARENT_SCOPE)
        return()
    endif ()

    get_target_property(runtime_dep ${arg_NAME} RUNTIME_DEP)
    if (NOT ("${runtime_dep}" STREQUAL "runtime_dep-NOTFOUND"))
        list(APPEND result_runtime_dep ${runtime_dep})
    endif()

    get_target_property(libs ${arg_NAME} LINK_LIBRARIES)
    if (NOT ("${libs}" STREQUAL "libs-NOTFOUND"))
        foreach(l ${libs})
            if (NOT TARGET ${l})
                continue()
            endif()

            get_target_property(type ${l} TYPE)
            if (type STREQUAL "SHARED_LIBRARY")
                list(APPEND result_runtime_dep $<TARGET_FILE:${l}>)
            endif ()

            list(APPEND result_dep_target ${l})
            exp_gather_target_runtime_dependencies_recurse(
                NAME ${l}
                OUT_RUNTIME_DEP temp_runtime_dep
                OUT_DEP_TARGET temp_dep_target
            )
            list(APPEND result_runtime_dep ${temp_runtime_dep})
            list(APPEND result_dep_target ${temp_dep_target})
        endforeach()
    endif()

    list(REMOVE_DUPLICATES result_runtime_dep)
    list(REMOVE_DUPLICATES result_dep_target)
    set(${arg_OUT_RUNTIME_DEP} ${result_runtime_dep} PARENT_SCOPE)
    set(${arg_OUT_DEP_TARGET} ${result_dep_target} PARENT_SCOPE)
endfunction()

# Every runtime file lands in the single shared per-sub-project Binaries directory. To keep the copies generator-agnostic
# and race-free, each destination is owned by exactly one copy step rather than being re-copied by every consumer:
#   - files referenced through a target ($<TARGET_FILE:...>) get a per-file owner created in the consumer's scope, so the
#     generator expression resolves where the target is visible (imported third-party targets such as Qt are
#     directory-scoped and would be invisible in a global aggregate target). A first-party owner additionally waits on its
#     producing target; merging these into one target is impossible because a build tool such as MirrorTool consumes a dll
#     while another dll's producer transitively depends on the tool, which would close a build cycle;
#   - prebuilt third-party files given as plain paths, plus resources, carry no target and are batched (deduplicated) into
#     one per-sub-project assets target whose copies run sequentially (see exp_finalize_dist_assets).
function(exp_add_runtime_dep_copy)
    set(options "")
    set(singleValueArgs KEY SRC PRODUCER OUTPUT_TARGET)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    string(MAKE_C_IDENTIFIER "${arg_KEY}" key_id)
    set(registry_property EXP_RUNTIME_DEP_COPY_${SUB_PROJECT_NAME}_${key_id})

    get_property(copy_target GLOBAL PROPERTY ${registry_property})
    if (NOT copy_target)
        exp_get_runtime_output_dir(OUTPUT out_dir)
        set(copy_target ${SUB_PROJECT_NAME}.CopyDll.${key_id})
        add_custom_target(
            ${copy_target}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${arg_SRC} ${out_dir}
        )
        set_target_properties(${copy_target} PROPERTIES FOLDER ${AUX_TARGETS_FOLDER})
        if (arg_PRODUCER)
            add_dependencies(${copy_target} ${arg_PRODUCER})
        endif ()
        set_property(GLOBAL PROPERTY ${registry_property} ${copy_target})
    endif ()

    set(${arg_OUTPUT_TARGET} ${copy_target} PARENT_SCOPE)
endfunction()

function(exp_schedule_dist_assets_finalize)
    get_property(scheduled GLOBAL PROPERTY EXP_DIST_ASSETS_SCHEDULED_${SUB_PROJECT_NAME})
    if (NOT scheduled)
        set_property(GLOBAL PROPERTY EXP_DIST_ASSETS_SCHEDULED_${SUB_PROJECT_NAME} TRUE)
        cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL exp_finalize_dist_assets "${SUB_PROJECT_NAME}")
    endif ()
endfunction()

function(exp_finalize_dist_assets sub_project)
    get_property(asset_files GLOBAL PROPERTY EXP_DIST_ASSET_FILES_${sub_project})
    get_property(asset_resources GLOBAL PROPERTY EXP_DIST_ASSET_RESOURCES_${sub_project})
    get_property(consumers GLOBAL PROPERTY EXP_DIST_ASSET_CONSUMERS_${sub_project})

    if (NOT asset_files AND NOT asset_resources)
        return()
    endif ()

    if (with_multi_config_generator)
        set(out_dir ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/${sub_project}/Binaries)
    else ()
        set(out_dir ${CMAKE_BINARY_DIR}/Dist/${sub_project}/Binaries)
    endif ()

    set(copy_commands COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir})
    if (asset_files)
        list(REMOVE_DUPLICATES asset_files)
        foreach (f ${asset_files})
            list(APPEND copy_commands COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} ${out_dir})
        endforeach ()
    endif ()
    if (asset_resources)
        list(REMOVE_DUPLICATES asset_resources)
        foreach (entry ${asset_resources})
            string(REPLACE "->" ";" pair "${entry}")
            list(GET pair 0 src)
            list(GET pair 1 dst)
            list(APPEND copy_commands COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src} ${out_dir}/${dst})
        endforeach ()
    endif ()

    set(copy_target ${sub_project}.CopyDistAssets)
    add_custom_target(${copy_target} ${copy_commands})
    set_target_properties(${copy_target} PROPERTIES FOLDER ${sub_project}/Aux)

    if (consumers)
        list(REMOVE_DUPLICATES consumers)
        foreach (consumer ${consumers})
            add_dependencies(${consumer} ${copy_target})
        endforeach ()
    endif ()
endfunction()

function(exp_process_runtime_dependencies)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME)
    set(multiValueArgs DEP_TARGET)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    exp_gather_target_runtime_dependencies_recurse(
        NAME ${arg_NAME}
        OUT_RUNTIME_DEP runtime_deps
        OUT_DEP_TARGET dep_targets
    )
    foreach (d ${arg_DEP_TARGET})
        list(APPEND runtime_deps $<TARGET_FILE:${d}>)
        exp_gather_target_runtime_dependencies_recurse(
            NAME ${d}
            OUT_RUNTIME_DEP dep_target_runtime_deps
            OUT_DEP_TARGET dep_dep_targets
        )
        list(APPEND runtime_deps ${dep_target_runtime_deps})
    endforeach ()

    if (runtime_deps)
        list(REMOVE_DUPLICATES runtime_deps)
    endif ()

    foreach (r ${runtime_deps})
        set(referenced "")
        if ("${r}" MATCHES "^\\$<TARGET_FILE:(.+)>$")
            set(referenced ${CMAKE_MATCH_1})
        endif ()

        if (referenced AND TARGET ${referenced})
            set(producer "")
            get_target_property(referenced_imported ${referenced} IMPORTED)
            if (NOT referenced_imported)
                set(producer ${referenced})
            endif ()

            exp_add_runtime_dep_copy(
                KEY ${r}
                SRC ${r}
                PRODUCER ${producer}
                OUTPUT_TARGET copy_target
            )
            add_dependencies(${arg_NAME} ${copy_target})
        else ()
            set_property(GLOBAL APPEND PROPERTY EXP_DIST_ASSET_FILES_${SUB_PROJECT_NAME} ${r})
        endif ()
    endforeach ()

    set_property(GLOBAL APPEND PROPERTY EXP_DIST_ASSET_CONSUMERS_${SUB_PROJECT_NAME} ${arg_NAME})
    exp_schedule_dist_assets_finalize()

    if (NOT arg_NOT_INSTALL AND runtime_deps)
        install(
            FILES ${runtime_deps} DESTINATION ${SUB_PROJECT_NAME}/Binaries
        )
    endif ()
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

        set_property(GLOBAL APPEND PROPERTY EXP_DIST_ASSET_RESOURCES_${SUB_PROJECT_NAME} "${src}->${dst}")

        cmake_path(SET dst_path NORMALIZE "${SUB_PROJECT_NAME}/Binaries/${dst}")
        cmake_path(GET dst_path PARENT_PATH dst_dir)
        if (NOT arg_NOT_INSTALL)
            install(FILES ${src} DESTINATION ${dst_dir})
        endif ()
    endforeach()

    set_property(GLOBAL APPEND PROPERTY EXP_DIST_ASSET_CONSUMERS_${SUB_PROJECT_NAME} ${arg_NAME})
    exp_schedule_dist_assets_finalize()
endfunction()

function(exp_gather_target_libs)
    set(options "")
    set(singleValueArgs NAME OUTPUT)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    string(REGEX MATCH "\\$\\<BUILD_INTERFACE.+\\>" match ${arg_NAME})
    if (match)
        set(${arg_OUTPUT} "" PARENT_SCOPE)
        return()
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
        list(APPEND inc_args \"${i}\")
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

    if (arg_DYNAMIC)
        list(APPEND dynamic_arg "-d")
    endif ()

    foreach (search_dir ${arg_SEARCH_DIR})
        file(GLOB_RECURSE input_header_files "${search_dir}/*.h")
        foreach (input_header_file ${input_header_files})
            string(REPLACE "${CMAKE_SOURCE_DIR}/" "" temp ${input_header_file})
            get_filename_component(dir ${temp} DIRECTORY)
            get_filename_component(filename ${temp} NAME_WE)

            set(output_source "${GENERATED_MIRROR_INFO_SRC_DIR}/${dir}/${filename}.generated.cpp")
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
    set_target_properties(${custom_target_name} PROPERTIES FOLDER ${AUX_TARGETS_FOLDER})
    set(${arg_OUTPUT_SRC} ${output_sources} PARENT_SCOPE)
    set(${arg_OUTPUT_TARGET_NAME} ${custom_target_name} PARENT_SCOPE)

    if (DEFINED arg_LIB)
        add_dependencies(${custom_target_name} ${arg_LIB})
    endif()
endfunction()

function(exp_get_runtime_output_dir)
    set(options "")
    set(singleValueArgs OUTPUT)
    set(multiValueArgs "")
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (with_multi_config_generator)
        set(${arg_OUTPUT} ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/${SUB_PROJECT_NAME}/Binaries PARENT_SCOPE)
    else ()
        set(${arg_OUTPUT} ${CMAKE_BINARY_DIR}/Dist/${SUB_PROJECT_NAME}/Binaries PARENT_SCOPE)
    endif ()
endfunction()

function(exp_add_executable)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME FOLDER)
    set(multiValueArgs SRC INC LINK LIB DEP_TARGET RES REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if (arg_NOT_INSTALL)
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
    if (DEFINED arg_FOLDER)
        set_target_properties(${arg_NAME} PROPERTIES FOLDER ${BASE_TARGETS_FOLDER}/${arg_FOLDER})
    else ()
        set_target_properties(${arg_NAME} PROPERTIES FOLDER ${BASE_TARGETS_FOLDER})
    endif ()

    exp_get_runtime_output_dir(OUTPUT runtime_output_dir)
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
        PRIVATE ${arg_LIB}
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

    if (MSVC)
        set_target_properties(
            ${arg_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY ${runtime_output_dir}
        )
    endif()

    if (NOT arg_NOT_INSTALL)
        if (export_sub_project_package)
            set(export_args EXPORT ${SUB_PROJECT_NAME}Targets)
        else ()
            set(export_args "")
        endif ()
        install(
            TARGETS ${arg_NAME}
            ${export_args}
            RUNTIME DESTINATION ${SUB_PROJECT_NAME}/Binaries
        )
        if (export_sub_project_package)
            export(
                TARGETS ${arg_NAME}
                NAMESPACE ${SUB_PROJECT_NAME}::
                APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
            )
        endif ()

        if (APPLE)
            install(CODE "execute_process(COMMAND install_name_tool -add_rpath @executable_path ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/$<TARGET_FILE_NAME:${arg_NAME}>)")
        endif ()
    endif ()
endfunction()

function(exp_add_library)
    set(options NOT_INSTALL)
    set(singleValueArgs NAME TYPE)
    set(multiValueArgs SRC PRIVATE_INC PUBLIC_INC PRIVATE_LINK PUBLIC_LINK PRIVATE_LIB PUBLIC_LIB PRIVATE_COMPILE_OPT PUBLIC_COMPILE_OPT REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    if ("${arg_TYPE}" STREQUAL "SHARED")
        list(APPEND arg_PUBLIC_INC ${GENERATED_API_HEADER_DIR}/${arg_NAME})
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
    set_target_properties(${arg_NAME} PROPERTIES FOLDER ${BASE_TARGETS_FOLDER})

    target_sources(
        ${arg_NAME}
        PRIVATE ${arg_SRC} ${generated_src}
    )
    if (with_multi_config_generator)
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
        get_filename_component(absolute_inc ${inc} ABSOLUTE)
        list(APPEND public_build_inc $<BUILD_INTERFACE:${absolute_inc}>)
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
    target_link_libraries(
        ${arg_NAME}
        PRIVATE ${arg_PRIVATE_LIB}
        PUBLIC ${arg_PUBLIC_LIB}
    )
    target_compile_options(
        ${arg_NAME}
        PRIVATE ${arg_PRIVATE_COMPILE_OPT}
        PUBLIC ${arg_PUBLIC_COMPILE_OPT}
    )

    if ("${arg_TYPE}" STREQUAL "SHARED")
        string(TOUPPER ${arg_NAME}_API api_name)
        string(REPLACE "-" "/" api_dir ${arg_NAME})

        generate_export_header(
            ${arg_NAME}
            EXPORT_MACRO_NAME ${api_name}
            EXPORT_FILE_NAME ${GENERATED_API_HEADER_DIR}/${arg_NAME}/${api_dir}/Api.h
        )
    endif()

    if (DEFINED arg_REFLECT)
        add_dependencies(${arg_NAME} ${generated_target})
    endif()

    if (NOT arg_NOT_INSTALL)
        foreach (inc ${arg_PUBLIC_INC})
            list(APPEND install_inc ${inc}/)
        endforeach ()
        install(
            DIRECTORY ${install_inc}
            DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Include
        )

        if (export_sub_project_package)
            set(export_args EXPORT ${SUB_PROJECT_NAME}Targets)
        else ()
            set(export_args "")
        endif ()
        install(
            TARGETS ${arg_NAME}
            ${export_args}
            ARCHIVE DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Lib
            LIBRARY DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Lib
            RUNTIME DESTINATION ${SUB_PROJECT_NAME}/Target/${arg_NAME}/Binaries
        )
        if (export_sub_project_package)
            export(
                TARGETS ${arg_NAME}
                NAMESPACE ${SUB_PROJECT_NAME}::
                APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
            )
        endif ()

        if ("${arg_TYPE}" STREQUAL "SHARED")
            install(
                FILES $<TARGET_FILE:${arg_NAME}>
                DESTINATION ${SUB_PROJECT_NAME}/Binaries
            )
        endif ()
    endif ()
endfunction()

function(exp_add_test)
    if (NOT BUILD_TEST)
        return()
    endif()

    set(options META)
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC LINK LIB DEP_TARGET RES REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    exp_add_executable(
        NAME ${arg_NAME}
        SRC ${arg_SRC}
        INC ${arg_INC}
        LINK ${arg_LINK}
        LIB Test ${arg_LIB}
        DEP_TARGET ${arg_DEP_TARGET}
        RES ${arg_RES}
        REFLECT ${arg_REFLECT}
        NOT_INSTALL
    )

    add_test(
        NAME ${arg_NAME}
        COMMAND ${arg_NAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${arg_NAME}>
    )
endfunction()

function(exp_add_benchmark)
    if (NOT BUILD_BENCHMARK)
        return()
    endif()

    set(options "")
    set(singleValueArgs NAME)
    set(multiValueArgs SRC INC LINK LIB DEP_TARGET RES REFLECT)
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    exp_add_executable(
        NAME ${arg_NAME}
        FOLDER Benchmark
        SRC ${arg_SRC}
        INC ${arg_INC}
        LINK ${arg_LINK}
        LIB Benchmark ${arg_LIB}
        DEP_TARGET ${arg_DEP_TARGET}
        RES ${arg_RES}
        REFLECT ${arg_REFLECT}
        NOT_INSTALL
    )
endfunction()

if (export_sub_project_package)
    install(
        EXPORT ${SUB_PROJECT_NAME}Targets
        FILE ${SUB_PROJECT_NAME}Targets.cmake
        NAMESPACE ${SUB_PROJECT_NAME}::
        DESTINATION ${SUB_PROJECT_NAME}
    )

    configure_package_config_file(
        ${CMAKE_CURRENT_LIST_DIR}/Config.cmake.in
        ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${SUB_PROJECT_NAME}/CMake
    )

    write_basic_package_version_file(
        ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}ConfigVersion.cmake
        VERSION ${SUB_PROJECT_VERSION_MAJOR}.${SUB_PROJECT_VERSION_MINOR}.${SUB_PROJECT_VERSION_PATCH}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Config.cmake
            ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${SUB_PROJECT_NAME}
    )

    file(GLOB_RECURSE preset_cmake_libs ${CMAKE_CURRENT_LIST_DIR}/*)
    foreach (cmake_lib ${preset_cmake_libs})
        file(
            COPY ${cmake_lib}
            DESTINATION ${CMAKE_BINARY_DIR}/CMake
        )
    endforeach ()
    install(
        FILES ${preset_cmake_libs}
        DESTINATION ${SUB_PROJECT_NAME}/CMake
    )

    if (DEFINED SUB_PROJECT_CMAKE_LIBS)
        foreach (cmake_lib ${SUB_PROJECT_CMAKE_LIBS})
            if (IS_ABSOLUTE ${cmake_lib})
                message(FATAL_ERROR "project cmake libs defined in SUB_PROJECT_CMAKE_LIBS should be relative path from project root")
            endif ()

            set(src_file ${CMAKE_SOURCE_DIR}/${cmake_lib})
            get_filename_component(binary_tree_dst_dir ${CMAKE_BINARY_DIR}/CMake/${cmake_lib} DIRECTORY)
            get_filename_component(install_tree_dst_dir ${SUB_PROJECT_NAME}/CMake/${cmake_lib} DIRECTORY)

            file(
                COPY ${src_file}
                DESTINATION ${binary_tree_dst_dir}
            )
            install(
                FILES ${src_file}
                DESTINATION ${install_tree_dst_dir}
            )
        endforeach ()
    endif ()
endif ()
