exp_add_library(
    NAME Test
    TYPE STATIC
    SRC Src/Main.cpp
    PUBLIC_INC Include
    PUBLIC_LIB Core
    PUBLIC_MERGE_LIB googletest
)