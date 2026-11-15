# Explosion

Before starting any work, agents must read `AGENTS.local.md` at the repository root if it exists.

## Code

- Prioritize sound abstractions and a single source of truth over a minimal diff.
- Prefer clear names and small functions; comment only when intent cannot be expressed in code.
- Avoid excessive defensive programming and validation. Write only meaningful tests. Do not overengineer compatibility
  or guard against scenarios that will not occur in practice.
- Use English exclusively throughout this project. Code, comments, and documentation must not contain other languages.
- Code has no line-length limit; wrap only for readability. Wrap comments at about 120 characters with roughly even lines.
- In headers, declare the complete interface first, then put out-of-line definitions in a second namespace block.
  Do not put function bodies inside class definitions. Put helper types in `Common::Internal`.
- Repeat class template-heads on out-of-line definitions; add a second template-head for member templates.
- In `.cpp` files, put translation-unit-local helpers in named `Xxx::Internal` namespaces and mark functions `static`
  to avoid unity-build ODR collisions. Namespace-scope `const`/`constexpr` variables need no `static`.
- Reference header layouts: `Delegate.h`, `Utility.h`, and `Result.h` in `Engine/Source/Common/Include/Common/`.

## CMake

- Keep global toolchain/compile settings in `CMake/Common.cmake` and target helpers in `CMake/Target.cmake`.
- Create targets through `exp_add_library`, `exp_add_executable`, `exp_add_test`, or `exp_add_benchmark`.
  Extend helpers instead of appending ad hoc `target_*` calls to their targets.
- Helpers are lower_snake_case `exp_*` functions using `cmake_parse_arguments(arg ...)`; read values as `arg_<KEYWORD>`.
- Keywords are UPPER_SNAKE_CASE; use `PRIVATE_*` / `PUBLIC_*` pairs for include/link/lib/compile-option inputs.
- Project options and cache variables are UPPER_SNAKE_CASE. Ordinary/local variables, including compiler-flag check
  results, are lower_snake_case without `EXP`/`EXP_` prefixes.
- Define platform/compiler flags as unprefixed 0/1 macros via `add_compile_definitions` in `Common.cmake`,
  e.g. `PLATFORM_WINDOWS` and `COMPILER_MSVC`.

## Build

- Default to Debug in `cmake-build-debug` with Ninja. Use `cmake-build-<config>` for other configurations when needed.
- Build incrementally; do not clear build directories or the Conan cache without a concrete reason.
- Never run concurrent configure/Conan processes against the same Conan cache.
- Keep machine-specific tool paths and parallelism in the ignored `AGENTS.local.md`.

## Git

- Commit subjects use `type: description` without a parenthesized scope, e.g. `fix: ...` or `refactor: ...`.
