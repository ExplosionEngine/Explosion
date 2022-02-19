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
    INC Sample ${glfw_INC} ${GLM_INC}
    LINK ${glfw_LINK}
    LIB RHI ${glfw_LIB}
)
