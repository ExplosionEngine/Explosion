set(MODULE_TARGET ParticleSystem)

project(${MODULE_TARGET})

file(GLOB_RECURSE SRC_FILES
    Src/*.cpp
    Src/*.h
    Effectors/*.cpp
    Effectors/*.h
    Emitters/*.cpp
    Emitters/*.h)

file(GLOB_RECURSE INC_FILES
    Include/*h
    Effectors/*.h
    Emitters/*.h)

list(APPEND INC_FILES ${GEN_PRE_DIR}/ParticleSystemApi.h)

exp_add_library(
    NAME ${MODULE_TARGET}
    TYPE STATIC
    SRCS ${SRC_FILES} ${INC_FILES}
    PUBLIC_INC_DIRS Include ${PROJECT_ROOT}/Engine/Include ${glm_INCS} Include
    LIBS CommonBase Math
)

add_subdirectory(Samples)

set(UNIT_TEST ParticleTest)
file(GLOB_RECURSE TEST_FILES Test/*)
exp_add_test(
    NAME ${UNIT_TEST}
    WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Test
    SRCS ${TEST_FILES}
    INC_DIRS ${googletest_INCS}
    LIBS ${googletest_LIBS} ${MODULE_TARGET}
)

generate_export_header(
    ${MODULE_TARGET}
    EXPORT_MACRO_NAME PARTICLE_API
    EXPORT_FILE_NAME ${GEN_PRE_DIR}/ParticleSystem/Api.h
)
