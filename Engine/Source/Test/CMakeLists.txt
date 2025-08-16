AddLibrary(
    NAME Test
    TYPE STATIC
    SRC Src/Main.cpp
    PUBLIC_INC Include
    LIB googletest Core
)