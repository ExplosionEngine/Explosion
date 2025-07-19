set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${SOURCES})
target_include_directories(Editor PRIVATE Include)
target_link_libraries(Editor PRIVATE Core RHI Runtime Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick)

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


# ---- begin qml -------------------------------------------------------------------------------------
set(EDITOR_QML_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Qml)
set(EDITOR_RESOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Resource)
get_filename_component(EDITOR_RESOURCE_ROOT_ABSOLUTE ${EDITOR_RESOURCE_ROOT} ABSOLUTE)

file(GLOB QML_SOURCES ${EDITOR_QML_ROOT}/*.qml)

list(
    APPEND SINGLETON_QML_SOURCES
        ETheme.qml)
target_compile_definitions(Editor PRIVATE -DSINGLETON_QML_SOURCES="${SINGLETON_QML_SOURCES}")

# QML only support placed in root dir
foreach (QML_SOURCE ${QML_SOURCES})
    get_filename_component(ALIAS ${QML_SOURCE} NAME)
    set_source_files_properties(${QML_SOURCE} PROPERTIES QT_RESOURCE_ALIAS ${ALIAS})
    if (${ALIAS} IN_LIST SINGLETON_QML_SOURCES)
        set_source_files_properties(${QML_SOURCE} PROPERTIES QT_QML_SINGLETON_TYPE true)
    endif ()
endforeach ()

add_custom_command(
    TARGET Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${EDITOR_RESOURCE_ROOT_ABSOLUTE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../Resource
)

qt_add_qml_module(
    Editor
    NO_CACHEGEN
    URI editor
    QML_FILES ${QML_SOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Generated/QmlModule
)
# ---- end qml ---------------------------------------------------------------------------------------
