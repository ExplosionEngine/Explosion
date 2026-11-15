set(imgui_stdlib_path ${imgui_PACKAGE_FOLDER_RELEASE}/res/misc/cpp)

file(GLOB_RECURSE editor_sources Src/*.cpp)
set(editor_platform_libs)
if (APPLE)
    list(APPEND editor_sources Src/Utils/MacosPlatformUtils.mm)
    set_source_files_properties(Src/Utils/MacosPlatformUtils.mm PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)
    list(APPEND editor_platform_libs "-framework Cocoa")
elseif (WIN32)
    list(APPEND editor_platform_libs ole32 shell32)
endif ()

exp_add_executable(
    NAME Editor
    SRC ${editor_sources} ${imgui_stdlib_path}/imgui_stdlib.cpp
    LIB Core Runtime glfw imgui::imgui ${editor_platform_libs}
    INC Include ${imgui_stdlib_path}
    DEP_TARGET ${platform_rhi_targets}
    REFLECT Include
)

file(GLOB editor_test_sources Test/*.cpp)
file(GLOB editor_project_sources Src/Project/*.cpp)
exp_add_test(
    NAME Editor.Test
    SRC ${editor_test_sources} Src/Asset/AssetFileSystem.cpp ${editor_project_sources} Src/ImGui/ImGuiDrawData.cpp
    LIB imgui::imgui
    INC Include
)

# ---- begin shaders/resources -----------------------------------------------------------------------
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
# ---- end shaders/resources -------------------------------------------------------------------------

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
