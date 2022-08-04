if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_EXT_LIBS
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreFoundation")
endif()

file(GLOB SOURCES Src/*.cpp)
AddExecutable(
    NAME Launcher
    SRC ${SOURCES}
    INC Include
    LIB Engine Renderer glfw clipp ${PLATFORM_EXT_LIBS}
)
