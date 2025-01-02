file(GLOB_RECURSE SOURCES Src/*.cpp)
AddLibrary(
    NAME Common
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB debugbreak cityhash taskflow rapidjson
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Common.Test
    INC Test
    SRC ${TEST_SOURCES}
)
