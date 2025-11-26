file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME RHI
    TYPE STATIC
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB Core
)
