file(GLOB_RECURSE SOURCES Src/*)
file(GLOB_RECURSE INCLUDES Include/*)

exp_add_library(
        NAME Reflection
        TYPE STATIC
        SRCS ${SOURCES} ${INCLUDES}
        PUBLIC_INC_DIRS
            Include
            ${rapidjson_INCS}
            ${entt_INCS}
        LIBS
            FileSystem
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
        NAME ReflectionTest
        TYPE STATIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        SRCS ${TEST_SOURCES}
        INC_DIRS ${googletest_INCS}
        LIBS Reflection ${googletest_LIBS}
)
