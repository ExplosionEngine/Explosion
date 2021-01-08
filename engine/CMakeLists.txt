# Vulkan
if (WIN32)
    if (NOT DEFINED ENV{VULKAN_SDK})
        message(FATAL_ERROR "found no env named VULKAN_SDK")
    endif()
    include_directories("$ENV{VULKAN_SDK}/Include")
elseif (UNIX)
    find_package(vulkan)
elseif (APPLE)
    find_package(vulkan)
endif()

# Engine
include_directories(include)
aux_source_directory(src ENGINE_SOURCES)
aux_source_directory(src/common ENGINE_COMMON_SOURCES)
aux_source_directory(src/core ENGINE_CORE_SOURCES)
aux_source_directory(src/driver ENGINE_DRIVER_SOURCES)
add_library(
    engine
    ${ENGINE_SOURCES}
    ${ENGINE_COMMON_SOURCES}
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_DRIVER_SOURCES})
if (WIN32)
    target_link_libraries(
        engine
        "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib"
    )
else()
    target_link_libraries(
        engine
        vulkan
    )
endif()
