file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME Common
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB debugbreak cityhash taskflow
)

file(GLOB TEST_SOURCES Test/*.cpp)
AddTest(
    NAME Common.Test
    SRC ${TEST_SOURCES}
)
