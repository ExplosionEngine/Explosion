file(GLOB RENDER_SAMPLE_SOURCES Main.cpp)

exp_add_executable(
    NAME RenderSample
    SRCS ${RENDER_SAMPLE_SOURCES}
    LIBS Engine SampleApplication FileSystem RHI
)