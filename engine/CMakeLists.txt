include_directories(include)
file(GLOB_RECURSE ENGINE_SOURCES src/**/*.cpp)
add_library(engine ${ENGINE_SOURCES})
target_link_libraries(engine Vulkan)
