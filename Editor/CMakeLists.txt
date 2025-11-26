set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick WebEngineWidgets
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_executable_hint MACOSX_BUNDLE)
    set(bundle_install_dest BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries)
    set(platform_fwk_dir ${QT_LIB_PREFIX}/lib)
endif ()

set(editor_includes Include)
set(editor_qt_libs Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick Qt6::WebEngineWidgets)
set(editor_libs Core RHI Runtime cpp-httplib ${editor_qt_libs})

foreach (QT_LIB ${editor_qt_libs})
    string(REPLACE "::" "" QT_RAW_LIB ${QT_LIB})
    list(APPEND editor_includes ${${QT_RAW_LIB}_INCLUDE_DIRS})
endforeach ()

exp_add_mirror_info_source_generation_target(
    NAME Editor
    OUTPUT_SRC EDITOR_MIRROR_GENERATED_SRC
    OUTPUT_TARGET_NAME EDITOR_MIRROR_GENERATED_TARGET
    SEARCH_DIR Include
    PRIVATE_INC ${editor_includes}
    LIB ${editor_libs}
    FRAMEWORK_DIR ${platform_fwk_dir}
)

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${platform_executable_hint} ${SOURCES} ${EDITOR_MIRROR_GENERATED_SRC})

get_cmake_property(GENERATOR_IS_MULTI_CONFIG GENERATOR_IS_MULTI_CONFIG)
if (${GENERATOR_IS_MULTI_CONFIG})
    set_target_properties(
        Editor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/$<CONFIG>/${SUB_PROJECT_NAME}/Binaries
    )
else ()
    set_target_properties(
        Editor PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Dist/${SUB_PROJECT_NAME}/Binaries
    )
endif ()

target_include_directories(Editor PRIVATE ${editor_includes})
target_link_libraries(Editor PRIVATE ${editor_libs})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(RHI_DEP_TARGETS RHI-DirectX12 RHI-Vulkan)
else()
    set(RHI_DEP_TARGETS RHI-Vulkan)
endif()
add_dependencies(Editor ${EDITOR_MIRROR_GENERATED_TARGET} ${RHI_DEP_TARGETS})

exp_process_runtime_dependencies(
    NAME Editor
)
install(
    TARGETS Editor
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries
    ${bundle_install_dest}
)
export(
    TARGETS Editor
    NAMESPACE ${SUB_PROJECT_NAME}::
    APPEND FILE ${CMAKE_BINARY_DIR}/${SUB_PROJECT_NAME}Targets.cmake
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(qt_win_deploy_executable ${QT_LIB_PREFIX}/bin/windeployqt.exe)
    add_custom_command(
        TARGET Editor POST_BUILD
        # ++ QT 6.9.1 Temporal Fix Start
        # #see https://bugreports.qt.io/browse/QTBUG-137542
        # NOTICE: seems cause some error on debug config, like render process crash after hover a remote link, please use DebugWithRelease before official fix at 6.9.2
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.pak ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.pak $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/resources/qtwebengine_devtools_resources.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources.pak $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/resources/qtwebengine_resources.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.pak $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/resources/qtwebengine_resources_100p.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.pak $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/resources/qtwebengine_resources_200p.pak
        # -- QT 6.9.1 Temporal Fix End
        COMMAND ${qt_win_deploy_executable} $<TARGET_FILE:Editor>
    )
    install(
        CODE "execute_process(COMMAND ${qt_win_deploy_executable} ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries/$<TARGET_FILE_NAME:Editor>)"
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(qt_mac_deploy_executable ${QT_LIB_PREFIX}/bin/macdeployqt)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${qt_mac_deploy_executable} $<TARGET_PROPERTY:Editor,RUNTIME_OUTPUT_DIRECTORY>/Editor.app -no-strip
    )
    install(
        CODE "execute_process(COMMAND ${qt_mac_deploy_executable} ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries/Editor.app -no-strip)"
    )
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    foreach (rhi_dep_target ${RHI_DEP_TARGETS})
        list(APPEND rhi_dep_target_copy_commands COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${rhi_dep_target}> $<TARGET_FILE_DIR:Editor>/../Frameworks/$<TARGET_FILE_NAME:${rhi_dep_target}>)
    endforeach ()

    add_custom_command(
        TARGET Editor POST_BUILD
        ${rhi_dep_target_copy_commands}
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
find_program(npm_executable NAMES npm.cmd npm REQUIRED NO_CACHE)
message("Perform web project npm install......")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ${npm_executable} install --no-fund --registry=https://registry.npmmirror.com
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Web
    RESULT_VARIABLE npm_install_result
    OUTPUT_VARIABLE npm_install_output
    ERROR_VARIABLE npm_install_error
)
if (${npm_install_result} STREQUAL "0")
    message(${npm_install_output})
else ()
    message(FATAL_ERROR ${npm_install_error})
endif ()

add_custom_target(
    Editor.Web
    COMMAND ${CMAKE_COMMAND} -E env ${npm_executable} run build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Web
    VERBATIM
)
add_dependencies(Editor Editor.Web)

add_custom_command(
    TARGET Editor.Web POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Web/dist $<TARGET_FILE_DIR:Editor>/Web
)
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Web/dist ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries/Web)")
endif ()
# ---- end web project -------------------------------------------------------------------------------
