file(GLOB SOURCES Src/*.cpp)
AddLibrary(
    NAME RHI-Dummy
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI
)
