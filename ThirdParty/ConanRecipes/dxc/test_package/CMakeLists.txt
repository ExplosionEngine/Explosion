cmake_minimum_required(VERSION 3.15)
project(test_package LANGUAGES CXX)

find_package(dxc REQUIRED)

add_definitions(-DPLATFORM_WINDOWS=$<IF:$<PLATFORM_ID:Windows>,1,0>)

add_executable(${PROJECT_NAME} test_package.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE dxc::dxc)
