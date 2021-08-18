set(MODULE_TARGET PassBuilder)

project(${MODULE_TARGET})

file(GLOB_RECURSE SRC_FILES Src/*.cpp Src/*.h)
file(GLOB_RECURSE INC_FILES Include/*h)
list(APPEND INC_FILES ${GEN_PRE_DIR}/PassBuilder/Api.h)

exp_add_library(
    NAME ${MODULE_TARGET}
    TYPE STATIC
    SRCS ${SRC_FILES} ${INC_FILES}
    PUBLIC_INC_DIRS Include ${PROJECT_ROOT}/Engine/Include ${rapidjson_INCS}
    LIBS FileSystem
)

set(UNIT_TEST PassTest)
file(GLOB_RECURSE TEST_FILES Test/*)
exp_add_test(
    NAME ${UNIT_TEST}
    WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Test
    SRCS ${TEST_FILES}
    INC_DIRS ${googletest_INCS}
    LIBS ${MODULE_TARGET} ${googletest_LIBS}
)

generate_export_header(${MODULE_TARGET}
    EXPORT_MACRO_NAME PASS_API
    EXPORT_FILE_NAME ${GEN_PRE_DIR}/PassBuilder/Api.h
)
