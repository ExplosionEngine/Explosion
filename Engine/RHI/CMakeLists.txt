file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Common
)

AddExecutable(
    NAME RHI-Triangle
    SRC Sample/Triangle.cpp
    INC ${glfw_INC}
    LINK ${glfw_LINK}
    LIB RHI ${glfw_LIB}
)
