option(BUILD_TEST "Build unit tests" ON)
option(ENABLE_TARGET_DEBUG_INFO "Enable debug info when add cmake targets" ON)

# AddExecutable
# Description: add a new executable target
# Params:
#  - NAME {Single} : name of target
#  - SRC  {List}   : sources of target
#  - INC  {List}   : private include directories of target
#  - LIB: {List}   : private libraries of target
function(AddExecutable)
    cmake_parse_arguments(PARAMS "" "NAME" "SRC;INC;LIB" ${ARGN})

    if (${ENABLE_TARGET_DEBUG_INFO})
        message("")
            message("[AddExecutable]")
            message(" - name: ${PARAMS_NAME}")
            message(" - sources: ${PARAMS_SRC}")
            message(" - includes: ${PARAMS_INC}")
            message(" - libraries: ${PARAMS_LIB}")
        message("")
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )
endfunction()

# AddLibrary
# Description: add a new library target
# Params:
#  - NAME        {Single}              : name of target
#  - TYPE        {"STATIC" | "SHARED"} : type of target
#  - SRC         {List}                : sources of target
#  - PRIVATE_INC {List}                : private include directories of target
#  - PUBLIC_INC  {List}                : public include directories of target
#  - LIB         {List}                : libraries of target
function(AddLibrary)
    cmake_parse_arguments(PARAMS "" "NAME;TYPE" "SRC;PRIVATE_INC;PUBLIC_INC;LIB" ${ARGN})

    if (${ENABLE_TARGET_DEBUG_INFO})
        message("")
            message("[AddLibrary]")
            message(" - name: ${PARAMS_NAME}")
            message(" - type: ${PARAMS_TYPE}")
            message(" - sources: ${PARAMS_SRC}")
            message(" - private include directories: ${PARAMS_PRIVATE_INC}")
            message(" - public include directories: ${PARAMS_PUBLIC_INC}")
            message(" - libraries: ${PARAMS_LIB}")
        message("")
    endif()

    add_library(
        ${PARAMS_NAME}
        ${PARAMS_TYPE}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_PRIVATE_INC}
        PUBLIC ${PARAMS_PUBLIC_INC}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )

    if (${MSVC})
        target_compile_options(
            ${PARAMS_NAME}
            PUBLIC $<IF:$<STREQUAL:${PARAMS_TYPE},"SHARED">,/MD$<$<CONFIG:Debug>:d>,/MT$<$<CONFIG:Debug>:d>>
        )
    endif()
endfunction()

# AddTest
# Description: add a mew test target
# Params:
#  - NAME        {Single} : name of target
#  - WORKING_DIR {Single} : working dir to running test command
#  - SRC         {List}   : sources of target
#  - INC         {List}   : private include directories of target
#  - LIB         {List}   : private libraries of target
function(AddTest)
    if (NOT ${BUILD_TEST})
        return()
    endif()

    cmake_parse_arguments(PARAMS "" "NAME;WORKING_DIR" "SRC;INC;LIB" ${ARGN})

    if (${ENABLE_TARGET_DEBUG_INFO})
        message("")
            message("[AddTest]")
            message(" - name: ${PARAMS_NAME}")
            message(" - working directory: ${PARAMS_WORKING_DIR}")
            message(" - sources: ${PARAMS_SRC}")
            message(" - includes: ${PARAMS_INC}")
            message(" - libraries: ${PARAMS_LIB}")
        message("")
    endif()

    add_executable(
        ${PARAMS_NAME}
        ${PARAMS_SRC}
    )
    target_include_directories(
        ${PARAMS_NAME}
        PRIVATE ${PARAMS_INC}
    )
    target_link_libraries(
        ${PARAMS_NAME}
        ${PARAMS_LIB}
    )
    add_test(
        NAME ${PARAMS_NAME}
        COMMAND ${PARAMS_NAME}
        WORKING_DIRECTORY ${PARAM_WORKING_DIR}
    )
endfunction()
