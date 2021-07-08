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
include_directories(../3rd/glm)
include_directories(${Vulkan_INCLUDE_DIRS})

if (WIN32)
    set(PLATFORM_SOURCES Src/RHI/Vulkan/Platform/VulkanWindows.cpp)
elseif(APPLE)
    set(PLATFORM_SOURCES Src/RHI/Vulkan/Platform/VulkanMacOS.mm)
elseif (UNIX AND NOT APPLE)
    set(PLATFORM_SOURCES Src/RHI/Vulkan/Platform/VulkanLinux.cpp)
endif()

if (APPLE)
    set(PLATFORM_LIBS "-framework Cocoa")
else()
    set(PLATFORM_LIBS "")
endif()
file(GLOB_RECURSE ENGINE_SOURCES Src/*.cpp)
exp_add_library(
    NAME Engine
    SRCS ${ENGINE_SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC_DIRS Include
    LIBS ${Vulkan_LIBRARIES} ${PLATFORM_LIBS} CommonBase JobSystem
)

add_subdirectory(Test)
add_subdirectory(Samples)
