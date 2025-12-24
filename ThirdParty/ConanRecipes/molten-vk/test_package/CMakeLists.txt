cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(MoltenVK REQUIRED)

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE molten-vk::molten-vk)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MoltenVK_INCLUDE_DIR}/../lib/libMoltenVK.dylib $<TARGET_FILE_DIR:${PROJECT_NAME}>/libMoltenVK.dylib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MoltenVK_INCLUDE_DIR}/../lib/MoltenVK_icd.json $<TARGET_FILE_DIR:${PROJECT_NAME}>/MoltenVK_icd.json
)
