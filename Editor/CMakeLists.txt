set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick WebEngineWidgets
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${SOURCES})
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
    set(QT_BUILDIN_QML_DIRECTORY ${QT_LIB_PREFIX}/qml)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${QT_WIN_DEPLOY_EXECUTABLE} $<TARGET_FILE:Editor> --qmldir ${QT_BUILDIN_QML_DIRECTORY} --verbose 0
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
