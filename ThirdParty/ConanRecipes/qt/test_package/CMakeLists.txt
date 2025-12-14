cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(Qt6 COMPONENTS Core Gui Widgets WebEngineWidgets REQUIRED)
set(QT_ROOT ${_Qt6_COMPONENT_PATH}/../..)

qt_standard_project_setup()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(platform_executable_hint MACOSX_BUNDLE)
endif ()

qt_add_executable(${PROJECT_NAME} ${platform_executable_hint} test_package.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::WebEngineWidgets)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(qt_win_deploy_executable ${QT_ROOT}/bin/windeployqt.exe)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${qt_win_deploy_executable} $<TARGET_FILE:${PROJECT_NAME}>
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(qt_mac_deploy_executable ${QT_ROOT}/bin/macdeployqt)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${qt_mac_deploy_executable} $<TARGET_BUNDLE_DIR:${PROJECT_NAME}> -no-strip
    )
endif ()
