set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick WebEngineWidgets
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_EXECUTABLE_HINT MACOSX_BUNDLE)
endif ()

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${PLATFORM_EXECUTABLE_HINT} ${SOURCES})
target_include_directories(Editor PRIVATE Include)
target_link_libraries(Editor PRIVATE Core RHI Runtime Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick Qt6::WebEngineWidgets)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PLATFORM_DEP_TARGET RHI-DirectX12 RHI-Vulkan)
else()
    set(PLATFORM_DEP_TARGET RHI-Vulkan)
endif()
add_dependencies(Editor ${PLATFORM_DEP_TARGET})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(QT_WIN_DEPLOY_EXECUTABLE ${QT_LIB_PREFIX}/bin/windeployqt.exe)
    add_custom_command(
        TARGET Editor POST_BUILD
        # ++ QT 6.9.1 Temporal Fix Start
        # #see https://bugreports.qt.io/browse/QTBUG-137542
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.pak ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.pak ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.debug.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_devtools_resources.pak ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/qtwebengine_devtools_resources.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources.pak ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/qtwebengine_resources.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_100p.pak ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/qtwebengine_resources_100p.pak
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_LIB_PREFIX}/resources/qtwebengine_resources_200p.pak ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/qtwebengine_resources_200p.pak
        # -- QT 6.9.1 Temporal Fix End
        COMMAND ${QT_WIN_DEPLOY_EXECUTABLE} $<TARGET_FILE:Editor>
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(QT_MAC_DEPLOY_EXECUTABLE ${QT_LIB_PREFIX}/bin/macdeployqt)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${QT_MAC_DEPLOY_EXECUTABLE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor.app
    )
endif ()

# ---- begin shaders ---------------------------------------------------------------------------------
GetEngineShaderResources(
    NAME Editor
    OUTPUT EDITOR_RESOURCES
)

file(GLOB_RECURSE SHADERS Shader/*.es*)
foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_ABSOLUTE ${SHADER} ABSOLUTE)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Shader ../Shader/Editor COPY_DST ${SHADER_ABSOLUTE})
    list(APPEND EDITOR_RESOURCES ${SHADER}->${COPY_DST})
endforeach ()

AddResourcesCopyCommand(
    NAME Editor
    RES ${EDITOR_RESOURCES}
)
# ---- end shaders -----------------------------------------------------------------------------------
