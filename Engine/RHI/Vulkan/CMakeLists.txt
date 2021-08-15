find_package(Vulkan REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DENABLE_VALIDATION_LAYER)
endif()

if (WIN32)
    set(PLATFORM_SOURCES Src/Platform/VulkanWindows.cpp)
elseif(APPLE)
    set(PLATFORM_SOURCES Src/Platform/VulkanMacOS.mm)
elseif (UNIX AND NOT APPLE)
    set(PLATFORM_SOURCES Src/Platform/VulkanLinux.cpp)
endif()

if (APPLE)
    set(PLATFORM_LIBS "-framework Cocoa")
else()
    set(PLATFORM_LIBS "")
endif()

file(GLOB_RECURSE SOURCES Src/*.cpp)
exp_add_library(
    NAME RHIVulkan
    TYPE SHARED
    SRCS ${SOURCES} ${PLATFORM_SOURCES}
    PUBLIC_INC_DIRS Include ${Vulkan_INCLUDE_DIRS} ${GEN_PRE_DIR}
    LIBS CommonBase RHI ${Vulkan_LIBRARIES} ${PLATFORM_LIBS}
)

generate_export_header(
    RHIVulkan
    EXPORT_MACRO_NAME RHI_VULKAN_API
    EXPORT_FILE_NAME ${GEN_PRE_DIR}/RHI/Vulkan/Api.h
)

file(GLOB_RECURSE TEST_SOURCES Test/*.cpp)
exp_add_test(
    NAME RHIVulkanTest
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SRCS ${TEST_SOURCES}
    INC_DIRS Include ${googletest_INCS}
    LIBS RHIVulkan ${googletest_LIBS}
)
