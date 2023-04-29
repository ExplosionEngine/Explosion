file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME AssetFactory
    PUBLIC_INC Include
    SRC ${SOURCES}
    LIB assimp-lib
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME AssetFactory.Test
    SRC ${TEST_SOURCES}
    LIB AssetFactory
    RES ${CMAKE_CURRENT_SOURCE_DIR}/Test/box.fbx->TestRes/box.fbx
)
