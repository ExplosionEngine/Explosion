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

# TODO config runtime dependencies in Windows

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
)
