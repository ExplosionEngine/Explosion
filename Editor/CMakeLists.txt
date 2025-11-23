set(CMAKE_PREFIX_PATH ${QT_LIB_PREFIX})
find_package(
    Qt6 ${QT_VERSION}
    COMPONENTS Core Gui Widgets Quick WebEngineWidgets
    REQUIRED
)

qt_standard_project_setup(REQUIRES ${QT_VERSION})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_EXECUTABLE_HINT MACOSX_BUNDLE)
    set(BUNDLE_INSTALL_DESTINATION BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries)
    set(PLATFORM_FRAMEWORK_DIR ${QT_LIB_PREFIX}/lib)
endif ()

set(EDITOR_INCLUDES Include)
set(EDITOR_QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Quick Qt6::WebEngineWidgets)
set(EDITOR_LIBS Core RHI Runtime cpp-httplib ${EDITOR_QT_LIBS})

foreach (QT_LIB ${EDITOR_QT_LIBS})
    string(REPLACE "::" "" QT_RAW_LIB ${QT_LIB})
    list(APPEND EDITOR_INCLUDES ${${QT_RAW_LIB}_INCLUDE_DIRS})
endforeach ()

exp_add_mirror_info_source_generation_target(
    NAME Editor
    OUTPUT_SRC EDITOR_MIRROR_GENERATED_SRC
    OUTPUT_TARGET_NAME EDITOR_MIRROR_GENERATED_TARGET
    SEARCH_DIR Include
    PRIVATE_INC ${EDITOR_INCLUDES}
    LIB ${EDITOR_LIBS}
    FRAMEWORK_DIR ${PLATFORM_FRAMEWORK_DIR}
)

file(GLOB_RECURSE SOURCES Src/*.cpp)
qt_add_executable(Editor ${PLATFORM_EXECUTABLE_HINT} ${SOURCES} ${EDITOR_MIRROR_GENERATED_SRC})

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

target_include_directories(Editor PRIVATE ${EDITOR_INCLUDES})
target_link_libraries(Editor PRIVATE ${EDITOR_LIBS})

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
        CODE "execute_process(COMMAND ${QT_WIN_DEPLOY_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries/$<TARGET_FILE_NAME:Editor>)"
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(QT_MAC_DEPLOY_EXECUTABLE ${QT_LIB_PREFIX}/bin/macdeployqt)
    add_custom_command(
        TARGET Editor POST_BUILD
        COMMAND ${QT_MAC_DEPLOY_EXECUTABLE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Editor.app -no-strip
    )
    install(
        CODE "execute_process(COMMAND ${QT_MAC_DEPLOY_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/${ENGINE_SUB_PROJECT_NAME}/Binaries/Editor.app -no-strip)"
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
get_engine_shader_resources(OUTPUT EDITOR_RESOURCES)

file(GLOB_RECURSE SHADERS Shader/*.es*)
foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_ABSOLUTE ${SHADER} ABSOLUTE)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Shader ../Shader/Editor COPY_DST ${SHADER_ABSOLUTE})
    list(APPEND EDITOR_RESOURCES ${SHADER}->${COPY_DST})
endforeach ()

file(GLOB_RECURSE RESOURCES Resource/*)
foreach (RESOURCE ${RESOURCES})
    get_filename_component(RESOURCE_ABSOLUTE ${RESOURCE} ABSOLUTE)
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR}/Resource ../Resource/Editor COPY_DST ${RESOURCE_ABSOLUTE})
    list(APPEND EDITOR_RESOURCES ${RESOURCE}->${COPY_DST})
endforeach ()

exp_add_resources_copy_command(
    NAME Editor
    RES ${EDITOR_RESOURCES}
)
# ---- end shaders -----------------------------------------------------------------------------------

# ---- begin web project -----------------------------------------------------------------------------
find_program(NPM_EXECUTABLE NAMES npm.cmd npm REQUIRED NO_CACHE)
message("Perform web project npm install......")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E env ${NPM_EXECUTABLE} install --no-fund --registry=https://registry.npmmirror.com
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
    COMMAND ${CMAKE_COMMAND} -E env ${NPM_EXECUTABLE} run build
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
