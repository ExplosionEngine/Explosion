cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(VulkanValidationLayers REQUIRED)

add_executable(${PROJECT_NAME} test_package.cpp)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VulkanValidationLayers_INCLUDE_DIR}/../bin/VkLayer_khronos_validation.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>/VkLayer_khronos_validation.dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VulkanValidationLayers_INCLUDE_DIR}/../bin/VkLayer_khronos_validation.json $<TARGET_FILE_DIR:${PROJECT_NAME}>/VkLayer_khronos_validation.json
    )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VulkanValidationLayers_INCLUDE_DIR}/../lib/libVkLayer_khronos_validation.dylib $<TARGET_FILE_DIR:${PROJECT_NAME}>/VkLayer_khronos_validation.dylib
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${VulkanValidationLayers_INCLUDE_DIR}/../share/vulkan $<TARGET_FILE_DIR:${PROJECT_NAME}>/vulkan
    )
else ()
    message(FATAL_ERROR "os ${CMAKE_SYSTEM_NAME} not supported yet in test package")
endif ()

