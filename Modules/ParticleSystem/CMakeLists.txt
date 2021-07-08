set(MODULE_TARGET ParticleSystem)

project(${MODULE_TARGET})

set(THIRD_PARTY_INC
    ${PROJECT_ROOT}/3rd/glm
)

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
        TYPE SHARED
        SRCS ${SRC_FILES} ${INC_FILES}
        PUBLIC_INC_DIRS Include
            ${PROJECT_ROOT}/Engine/Include
            ${THIRD_PARTY_INC}
            Include
        LIBS explosion Math
)

add_subdirectory(Samples)

set(UNIT_TEST ParticleTest)
file(GLOB_RECURSE TEST_FILES Test/*)
exp_add_test(
        NAME ${UNIT_TEST}
        WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Test
        SRCS ${TEST_FILES}
        INC_DIRS ${PROJECT_ROOT}/3rd/googletest/googletest/include
        LIBS ${MODULE_TARGET} gtest
)

generate_export_header(${MODULE_TARGET}
        EXPORT_MACRO_NAME PARTICLE_API
        EXPORT_FILE_NAME ${GEN_PRE_DIR}/ParticleSystemApi.h
        )
