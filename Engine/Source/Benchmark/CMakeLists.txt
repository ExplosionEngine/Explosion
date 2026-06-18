exp_add_library(
    NAME Benchmark
    TYPE STATIC
    SRC Src/Main.cpp
    PUBLIC_LIB benchmark::benchmark
)
