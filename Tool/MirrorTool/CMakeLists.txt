file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME MirrorTool.Static
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Mirror
    PUBLIC_MERGE_LIB llvm-clang clipp
    NOT_INSTALL
)

file(GLOB exe_sources ExeSrc/*.cpp)
exp_add_executable(
    NAME MirrorTool
    SRC ${exe_sources}
    INC Include
    LIB MirrorTool.Static
)

exp_add_test(
    NAME MirrorTool.Test
    SRC Test/Main.cpp
    INC Include
    LIB MirrorTool.Static
    RES
        ${CMAKE_CURRENT_SOURCE_DIR}/Test/MirrorToolInput.h->../Test/Resource/Mirror/MirrorToolInput.h
        ${CMAKE_SOURCE_DIR}/${ENGINE_SUB_PROJECT_NAME}/Source/Mirror/Include/Mirror/Meta.h->../Test/Resource/Mirror/Mirror/Meta.h
)
