set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets
    REQUIRED
)

qt_standard_project_setup()

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${SOURCES})
target_include_directories(Editor PRIVATE Include)
target_link_libraries(Editor PRIVATE Core RHI Runtime Qt6::Core Qt6::Gui Qt6::Widgets)

set_target_properties(
    Editor PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
