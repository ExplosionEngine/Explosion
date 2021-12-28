file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RenderGraph
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Common
)
