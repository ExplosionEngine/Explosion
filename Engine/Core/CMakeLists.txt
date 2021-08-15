if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DENABLE_VALIDATION_LAYER)
endif()

find_package(Vulkan REQUIRED)

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
    TYPE STATIC
    SRCS ${ENGINE_SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC_DIRS Include ${entt_INCS} ${Vulkan_INCLUDE_DIRS}
    LIBS ${Vulkan_LIBRARIES} ${PLATFORM_LIBS} CommonBase JobSystem
)

add_subdirectory(Test)
add_subdirectory(Samples)
