set(MODULE_TARGET ShaderBuilder)
project(${MODULE_TARGET})

set(THIRD_PARTY_INC
    ${rapidjson_INCS}
    ${glslang_INCS}
)

set(THIRD_PARTY_LIB ${glslang_LIBS})

file(GLOB_RECURSE SRC_FILES Src/*.cpp Src/*.h)
file(GLOB_RECURSE INC_FILES Include/*h)
file(GLOB_RECURSE TOOL_FILES Tool/*)

exp_add_library(
    NAME ${MODULE_TARGET}
    TYPE STATIC
    SRCS ${SRC_FILES} ${INC_FILES}
    PUBLIC_INC_DIRS
        Include
        ${PROJECT_ROOT}/Engine/Include
        ${THIRD_PARTY_INC}
    LIBS Engine IO ${THIRD_PARTY_LIB}
)

exp_add_executable(
    NAME ${MODULE_TARGET}Tool
    SRCS ${TOOL_FILES}
    LIBS ${MODULE_TARGET}
)