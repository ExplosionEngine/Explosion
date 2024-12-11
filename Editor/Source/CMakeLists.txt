file(GLOB_RECURSE RESOURCES ../Resource/*)
foreach(R ${RESOURCES})
    string(REGEX REPLACE ".*Resource/" "" FILENAME ${R})
    list(APPEND FINAL_RESOURCES ${R}->../Resource/${FILENAME})
endforeach()

file(GLOB_RECURSE SOURCES Src/*.cpp)
AddExecutable(
    NAME Editor
    SRC ${SOURCES}
    INC Include
    LIB Qt Core RHI Runtime
    RES ${FINAL_RESOURCES}
)

set_target_properties(
    Editor PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
