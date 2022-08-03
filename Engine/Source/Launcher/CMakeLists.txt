file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME Launcher
    SRC ${SOURCES}
    INC Include
    LIB Engine Renderer glfw
)
