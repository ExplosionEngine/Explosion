# Math SIMD baseline. The math types are header-only, so the option is PUBLIC: every consumer instantiates them and must
# share one ISA baseline. The simd backend only relies on the SSE2/NEON baseline (present on every CPU this engine
# targets), so it is always on; this just lifts the compiler's instruction-set baseline where that helps codegen. MSVC
# x64 implies SSE2 and has no /arch:SSE4.2; on gcc/clang lift the baseline to SSE4.2 where supported (the check fails and
# is skipped on non-x86 targets such as aarch64, which already ship NEON).
if (NOT MSVC)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-msse4.2" has_msse42)
    if (has_msse42)
        set(math_public_compile_opt -msse4.2)
    endif ()
endif ()

file(GLOB_RECURSE sources Src/*.cpp)
exp_add_library(
    NAME Common
    TYPE STATIC
    SRC ${sources}
    PUBLIC_INC Include
    PUBLIC_LIB rapidjson cityhash::cityhash
    PRIVATE_LIB debugbreak::debugbreak
    PUBLIC_COMPILE_OPT ${math_public_compile_opt}
)

file(GLOB test_sources Test/*.cpp)
exp_add_test(
    NAME Common.Test
    INC Test
    SRC ${test_sources}
    LIB Common
)

# exp_add_benchmark early-returns when BUILD_BENCHMARK is OFF, so this is safe to declare unconditionally.
file(GLOB benchmark_sources Benchmark/*.cpp)
exp_add_benchmark(
    NAME Common.Benchmark
    SRC ${benchmark_sources}
    LIB Common
)
