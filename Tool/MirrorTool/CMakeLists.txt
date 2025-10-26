file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME MirrorTool.Static
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB clipp llvm-clang Mirror
    NOT_INSTALL
)

file(GLOB EXE_SOURCES ExeSrc/*.cpp)
exp_add_executable(
    NAME MirrorTool
    SRC ${EXE_SOURCES}
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
