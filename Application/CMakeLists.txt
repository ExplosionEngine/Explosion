find_package(Vulkan)
if (${Vulkan_FOUND})
    message("-- found Vulkan library")
else()
    message(FATAL_ERROR "-- can not find the Vulkan lib, make sure you have installed the Vulkan SDK")
endif()

include_directories(${Vulkan_INCLUDE_DIRS})

file(GLOB APPLICATION_SOURCES Src/*.cpp)
add_library(application SHARED ${APPLICATION_SOURCES})
target_include_directories(application PUBLIC include)
target_link_libraries(application glfw ${Vulkan_LIBRARIES})
