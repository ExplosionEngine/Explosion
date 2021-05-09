
set(PLUG_TARGET PassBuilder)

project(${PLUG_TARGET})

set(THIRD_PARTY_INC
    ${PROJECT_ROOT}/3rd/rapidjson/include
)

include_directories(Include)
include_directories(${PROJECT_ROOT}/Engine/Include)
include_directories(${THIRD_PARTY_INC})

file(GLOB_RECURSE SRC_FILES Src/*.cpp Src/*.h)
file(GLOB_RECURSE INC_FILES Include/*h)
list(APPEND INC_FILES ${GEN_PRE_DIR}/PassBuilderApi.h)

add_library(${PLUG_TARGET} SHARED
    ${SRC_FILES}
    ${INC_FILES}
)

generate_export_header(${PLUG_TARGET}
    EXPORT_MACRO_NAME PASS_API
    EXPORT_FILE_NAME ${GEN_PRE_DIR}/PassBuilderApi.h
)

target_link_libraries(${PLUG_TARGET}
    explosion
)

## TEST
if (BUILD_TEST)
    enable_testing()
    file(GLOB_RECURSE TEST_FILES Test/*)

    include_directories(${PROJECT_ROOT}/3rd/googletest/include/gtest)

    set(UNIT_TEST PassTest)

    add_executable(${UNIT_TEST}
        ${TEST_FILES}
    )

    target_link_libraries(${UNIT_TEST}
        ${PLUG_TARGET}
        gtest
    )

    add_custom_command(
        TARGET ${UNIT_TEST}
        COMMENT "Run tests"
        POST_BUILD 
        COMMAND ${UNIT_TEST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Test
   )
endif ()