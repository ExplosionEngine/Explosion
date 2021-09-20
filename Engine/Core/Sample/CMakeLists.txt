file(GLOB RENDER_SAMPLE_SOURCES Main.cpp)

exp_add_executable(
    NAME EngineSample
    SRCS ${RENDER_SAMPLE_SOURCES}
    LIBS Engine SampleApplication RHI
)