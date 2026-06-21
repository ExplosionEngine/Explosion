find_package(
    Qt6
    COMPONENTS Core Gui Widgets WebEngineWidgets
    REQUIRED
)
set(QT_ROOT ${QT6_INSTALL_PREFIX})

qt_standard_project_setup()

if (APPLE)
    set(platform_executable_hint MACOSX_BUNDLE)
    set(bundle_install_dest BUNDLE DESTINATION ${SUB_PROJECT_NAME}/Binaries)
    set(platform_fwk_dir ${QT_ROOT}/lib)
endif ()

set(editor_qt_libs Qt6::Core Qt6::Gui Qt6::Widgets Qt6::WebEngineWidgets)
set(editor_libs Core RHI Runtime httplib::httplib ${editor_qt_libs})

exp_add_mirror_info_source_generation_target(
    NAME Editor
    OUTPUT_SRC EDITOR_MIRROR_GENERATED_SRC
    OUTPUT_TARGET_NAME EDITOR_MIRROR_GENERATED_TARGET
    SEARCH_DIR Include
    PRIVATE_INC Include
    LIB ${editor_libs}
    FRAMEWORK_DIR ${platform_fwk_dir}
)

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${platform_executable_hint} ${SOURCES} ${EDITOR_MIRROR_GENERATED_SRC})
set_target_properties(Editor PROPERTIES FOLDER ${BASE_TARGETS_FOLDER})

exp_get_runtime_output_dir(OUTPUT runtime_output_dir)
set_target_properties(
    Editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${runtime_output_dir}
)

target_include_directories(Editor PRIVATE Include)
target_link_libraries(Editor PRIVATE ${editor_libs})

add_dependencies(Editor ${EDITOR_MIRROR_GENERATED_TARGET} ${platform_rhi_targets})

exp_process_runtime_dependencies(
    NAME Editor
    DEP_TARGET ${platform_rhi_targets}
)
install(
    TARGETS Editor
    RUNTIME DESTINATION ${SUB_PROJECT_NAME}/Binaries
    ${bundle_install_dest}
)
export(
    TARGETS Editor
    NAMESPACE ${SUB_PROJECT_NAME}::
    APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
)

if (WIN32)
    set(qt_win_deploy_executable ${QT_ROOT}/bin/windeployqt.exe)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${qt_win_deploy_executable} $<TARGET_FILE:Editor>
    )
    install(
        CODE "execute_process(COMMAND ${qt_win_deploy_executable} ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/$<TARGET_FILE_NAME:Editor>)"
    )
elseif (APPLE)
    set(qt_mac_deploy_executable ${QT_ROOT}/bin/macdeployqt)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${qt_mac_deploy_executable} $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/Editor.app -no-strip
    )
    install(
        CODE "execute_process(COMMAND ${qt_mac_deploy_executable} ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/Editor.app -no-strip)"
    )
endif ()

# ---- begin shaders ---------------------------------------------------------------------------------
get_engine_shader_resources(OUTPUT editor_resources)

file(GLOB_RECURSE shaders Shader/*.es*)
foreach (shader ${shaders})
    get_filename_component(shader_absolute ${shader} ABSOLUTE)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Shader ../Shader/Editor copy_dst ${shader_absolute})
    list(APPEND editor_resources ${shader}->${copy_dst})
endforeach ()

file(GLOB_RECURSE resources Resource/*)
foreach (resource ${resources})
    get_filename_component(resource_absolute ${resource} ABSOLUTE)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Resource ../Resource/Editor copy_dst ${resource_absolute})
    list(APPEND editor_resources ${resource}->${copy_dst})
endforeach ()

exp_add_resources_copy_command(
    NAME Editor
    RES ${editor_resources}
)
# ---- end shaders -----------------------------------------------------------------------------------

# ---- begin web project -----------------------------------------------------------------------------
if (WIN32)
    find_program(npm_executable NAMES npm.cmd npm REQUIRED NO_CACHE)
else ()
    find_program(npm_executable NAMES npm REQUIRED NO_CACHE)
endif ()

add_custom_target(
    Editor.Web
    COMMAND ${CMAKE_COMMAND} -E env ${npm_executable} install --no-fund --registry=https://registry.npmmirror.com
    COMMAND ${CMAKE_COMMAND} -E env ${npm_executable} run build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Web
    VERBATIM
)
set_target_properties(Editor.Web PROPERTIES FOLDER ${AUX_TARGETS_FOLDER})
add_dependencies(Editor Editor.Web)

add_custom_command(
    TARGET Editor.Web POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Web/dist $<TARGET_FILE_DIR:Editor>/Web
)
if (NOT APPLE)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Web/dist ${CMAKE_INSTALL_PREFIX}/${SUB_PROJECT_NAME}/Binaries/Web)")
endif ()
# ---- end web project -------------------------------------------------------------------------------

# ---- begin test project ----------------------------------------------------------------------------
# Render the Default project template into TestProject/ at configure time, so local and CI builds can build it
# against the installed engine. The variable names must match the @placeholders@ in the template.
set(projectName ExplosionTest)
set(cmakeMinVersion 3.25)

set(project_template_dir ${CMAKE_CURRENT_SOURCE_DIR}/Resource/ProjectTemplates/Default)
set(test_project_dir ${CMAKE_SOURCE_DIR}/TestProject)
file(GLOB_RECURSE project_template_files RELATIVE ${project_template_dir} ${project_template_dir}/*)
foreach (template_file ${project_template_files})
    if (template_file MATCHES "\\.tpl$")
        string(REGEX REPLACE "\\.tpl$" "" rendered_file ${template_file})
        configure_file(${project_template_dir}/${template_file} ${test_project_dir}/${rendered_file} @ONLY)
    else ()
        configure_file(${project_template_dir}/${template_file} ${test_project_dir}/${template_file} COPYONLY)
    endif ()
endforeach ()
# ---- end test project ------------------------------------------------------------------------------
