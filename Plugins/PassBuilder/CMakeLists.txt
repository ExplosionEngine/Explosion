
set(PLUG_TARGET PassBuilder)

project(${PLUG_TARGET})

set(THIRD_PARTY_INC
    ${PROJECT_ROOT}/3rd/json
)

include_directories(Include)
include_directories(${PROJECT_ROOT}/Engine/Include)

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