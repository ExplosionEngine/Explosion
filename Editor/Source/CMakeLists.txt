file(GLOB_RECURSE SOURCES Src/*.cpp)
AddExecutable(
    NAME Editor
    SRC ${SOURCES}
    INC Include
    LIB Qt Core RHI Rendering Runtime
)
set_target_properties(
    Editor PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
