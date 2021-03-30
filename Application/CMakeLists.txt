file(GLOB APPLICATION_SOURCES Src/*.cpp)
add_library(application SHARED ${APPLICATION_SOURCES})
target_include_directories(application PUBLIC include)
