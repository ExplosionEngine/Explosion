if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DENABLE_VALIDATION_LAYER)
endif()

find_package(Vulkan)
if (${Vulkan_FOUND})
    message("-- found Vulkan library")
else()
    message(FATAL_ERROR "-- can not find the Vulkan lib, make sure you have installed the Vulkan SDK")
endif()

include_directories(../3rd/entt/single_include)
include_directories(${Vulkan_INCLUDE_DIRS})

if (WIN32)
    set(PLATFORM_SOURCES Src/Driver/Platform/Windows.cpp)
elseif(LINUX)
elseif(APPLE)
    set(PLATFORM_SOURCES Src/Driver/Platform/MacOS.mm)
endif()

file(GLOB_RECURSE ENGINE_SOURCES Src/*.cpp)
add_library(explosion SHARED ${ENGINE_SOURCES} ${PLATFORM_SOURCES})

target_include_directories(explosion PUBLIC Include)
target_link_libraries(explosion ${Vulkan_LIBRARIES})
