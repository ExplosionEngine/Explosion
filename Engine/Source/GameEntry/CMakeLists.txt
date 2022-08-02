file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME GameEntry
    SRC ${SOURCES}
    INC Include
    LIB Runtime glfw
)
