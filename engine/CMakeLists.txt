find_package(Vulkan)
if (${Vulkan_FOUND})
    message("-- found Vulkan library")
else()
    message(FATAL_ERROR "-- can not find the Vulkan lib, make sure you have installed the Vulkan SDK")
endif()

include_directories(${Vulkan_INCLUDE_DIRS})
include_directories(include)

file(GLOB_RECURSE ENGINE_SOURCES src/**/*.cpp)
add_library(engine ${ENGINE_SOURCES})
target_link_libraries(engine ${Vulkan_LIBRARIES})
