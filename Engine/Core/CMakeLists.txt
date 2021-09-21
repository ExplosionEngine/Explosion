file(GLOB_RECURSE ENGINE_SOURCES Src/*.cpp)
exp_add_library(
    NAME Engine
    TYPE STATIC
    SRCS ${ENGINE_SOURCES}
    PUBLIC_INC_DIRS Include ${entt_INCS}
    LIBS CommonBase Math JobSystem RHI
)

add_subdirectory(Sample)
add_subdirectory(Test)
