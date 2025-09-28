file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME RHI-Dummy
    TYPE SHARED
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB RHI
)
