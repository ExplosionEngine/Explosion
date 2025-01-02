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
    list(
        APPEND RUNTIME_DEP
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/plugins/platforms/qwindows$<IF:$<CONFIG:Debug>,d,>.dll->platforms/qwindows$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/plugins/tls->tls
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/qml->qml
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Core$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Widgets$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Quick$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Qml$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6QmlModels$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6QmlWorkerScript$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Network$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6Gui$<IF:$<CONFIG:Debug>,d,>.dll
            ${Qt_SOURCE_DIR}/${QT_VERSION}/msvc2019_64/bin/Qt6OpenGL$<IF:$<CONFIG:Debug>,d,>.dll
    )
    set_target_properties(Editor PROPERTIES RUNTIME_DEP "${RUNTIME_DEP}")
    AddRuntimeDependenciesCopyCommand(NAME Editor)
endif ()

file(GLOB_RECURSE QML_SOURCES QML/*.qml)
file(GLOB_RECURSE RESOURCES Resource/*)

list(APPEND RESOURCES_PENDING_SET_ALIAS ${QML_SOURCES} ${RESOURCES})
foreach (RESOURCE ${RESOURCES_PENDING_SET_ALIAS})
    get_filename_component(FILENAME ${RESOURCE} NAME)
    set_source_files_properties(${RESOURCE} PROPERTIES QT_RESOURCE_ALIAS ${FILENAME})
endforeach ()

qt_add_qml_module(
    Editor
    URI editor
    QML_FILES ${QML_SOURCES}
    RESOURCES ${RESOURCES}
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Generated/QmlModule
)
