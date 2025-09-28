file(GLOB SOURCES Src/*.cpp)
exp_add_library(
    NAME RHI
    TYPE STATIC
    SRC ${SOURCES}
    PUBLIC_INC Include
    LIB Core
)
