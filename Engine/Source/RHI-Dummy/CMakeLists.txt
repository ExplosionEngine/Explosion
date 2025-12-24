file(GLOB sources Src/*.cpp)
exp_add_library(
    NAME RHI-Dummy
    TYPE SHARED
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB RHI
)
