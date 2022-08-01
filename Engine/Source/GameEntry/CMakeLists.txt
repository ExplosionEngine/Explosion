file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME GameEntry
    SRC ${SOURCES}
    LIB Runtime glfw
)
