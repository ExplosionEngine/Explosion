set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick WebEngineWidgets
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_EXECUTABLE_HINT MACOSX_BUNDLE)
    set(BUNDLE_INSTALL_DESTINATION BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Binaries)
endif ()

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${PLATFORM_EXECUTABLE_HINT} ${SOURCES})
target_include_directories(Editor PRIVATE Include)
target_include_directories(Editor PRIVATE ${CPP_HTTP_LIB_SOURCE_DIR})
LinkLibraries(
    NAME Editor
    LIB Core RHI Runtime Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick Qt6::WebEngineWidgets
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(RHI_DEP_TARGETS RHI-DirectX12 RHI-Vulkan)
else()
    set(RHI_DEP_TARGETS RHI-Vulkan)
endif()
add_dependencies(Editor ${RHI_DEP_TARGETS})

AddRuntimeDependenciesCopyCommand(
    NAME Editor
)
install(
    TARGETS Editor
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/Engine/Binaries
    ${BUNDLE_INSTALL_DESTINATION}
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(QT_WIN_DEPLOY_EXECUTABLE ${QT_LIB_PREFIX}/bin/windeployqt.exe)
    add_custom_command(
        TARGET Editor POST_BUILD
        # ++ QT 6.9.1 Temporal Fix Start
        # #see https://bugreports.qt.io/browse/QTBUG-137542
        # NOTICE: seems cause some error on debug config, like render process crash after hover a remote link, please use DebugWithRelease before official fix at 6.9.2
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
    install(
        CODE "execute_process(COMMAND ${QT_WIN_DEPLOY_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/Engine/Binaries/$<TARGET_FILE_NAME:Editor>)"
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(QT_MAC_DEPLOY_EXECUTABLE ${QT_LIB_PREFIX}/bin/macdeployqt)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${QT_MAC_DEPLOY_EXECUTABLE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor.app -no-strip
    )
    install(
        CODE "execute_process(COMMAND ${QT_MAC_DEPLOY_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/Engine/Binaries/Editor.app -no-strip)"
    )
endif ()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    foreach (RHI_DEP_TARGET ${RHI_DEP_TARGETS})
        list(APPEND RHI_DEP_TARGET_COPY_COMMANDS COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${RHI_DEP_TARGET}> $<TARGET_FILE_DIR:Editor>/../Frameworks/$<TARGET_FILE_NAME:${RHI_DEP_TARGET}>)
    endforeach ()

    add_custom_command(
        TARGET Editor POST_BUILD
        ${RHI_DEP_TARGET_COPY_COMMANDS}
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

# ---- begin web project -----------------------------------------------------------------------------
set(NODEJS_HOME ${3RD_SOURCE_DIR}/Nodejs-${CMAKE_SYSTEM_NAME}-${NODEJS_VERSION})
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(NODEJS_PATH ${NODEJS_HOME})
    set(NPM_EXECUTABLE npm.cmd)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(NODEJS_BIN_PATH ${NODEJS_HOME}/bin)
    set(NODEJS_MODULES_PATH ${NODEJS_HOME}/lib/node_modules)
    file(REMOVE ${NODEJS_BIN_PATH}/corepack)
    file(REMOVE ${NODEJS_BIN_PATH}/npm)
    file(REMOVE ${NODEJS_BIN_PATH}/npx)
    execute_process(
        COMMAND ln -s ${NODEJS_MODULES_PATH} ${NODEJS_BIN_PATH}/node_modules
        RESULT_VARIABLE LINK_RESULT
        OUTPUT_VARIABLE LINK_OUTPUT
        ERROR_VARIABLE LINK_ERROR
    )
    execute_process(
        COMMAND ln -s ${NODEJS_MODULES_PATH}/npm/bin/npm ${NODEJS_BIN_PATH}/npm
        RESULT_VARIABLE LINK_RESULT
        OUTPUT_VARIABLE LINK_OUTPUT
        ERROR_VARIABLE LINK_ERROR
    )
    execute_process(
        COMMAND ln -s ${NODEJS_MODULES_PATH}/npm/bin/npx ${NODEJS_BIN_PATH}/npx
        RESULT_VARIABLE LINK_RESULT
        OUTPUT_VARIABLE LINK_OUTPUT
        ERROR_VARIABLE LINK_ERROR
    )
    set(NODEJS_PATH "$ENV{PATH}:${NODEJS_BIN_PATH}")
    set(NPM_EXECUTABLE npm)
else ()
    message(FATAL_ERROR "Unsupported OS")
endif ()

message("Perform web project npm install......")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env PATH=${NODEJS_PATH} ${NPM_EXECUTABLE} install --no-fund --registry=https://registry.npmmirror.com
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Web
    RESULT_VARIABLE NPM_INSTALL_RESULT
    OUTPUT_VARIABLE NPM_INSTALL_OUTPUT
    ERROR_VARIABLE NPM_INSTALL_ERROR
)
if (${NPM_INSTALL_RESULT} STREQUAL "0")
    message(${NPM_INSTALL_OUTPUT})
else ()
    message(FATAL_ERROR ${NPM_INSTALL_ERROR})
endif ()

add_custom_target(
    Editor.Web
    COMMAND ${CMAKE_COMMAND} -E env PATH=${NODEJS_PATH} ${NPM_EXECUTABLE} run build
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Web
    VERBATIM
)
add_dependencies(Editor Editor.Web)

add_custom_command(
    TARGET Editor.Web POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_CURRENT_SOURCE_DIR}/Web/dist $<TARGET_FILE_DIR:Editor>/Web
)
# ---- end web project -------------------------------------------------------------------------------
