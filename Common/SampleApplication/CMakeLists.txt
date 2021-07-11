find_package(Vulkan)
if (${Vulkan_FOUND})
    message("-- found Vulkan library")
else()
    message(FATAL_ERROR "-- can not find the Vulkan lib, make sure you have installed the Vulkan SDK")
endif()

if (${APPLE})
    set(PLATFORM_LIBS "-framework IOKit")
else()
    set(PLATFORM_LIBS "")
endif()

file(GLOB APPLICATION_SOURCES Src/*.cpp)
exp_add_library(
    NAME SampleApplication
    TYPE STATIC
    SRCS ${APPLICATION_SOURCES}
    PRIVATE_INC_DIRS ${Vulkan_INCLUDE_DIRS}
    PUBLIC_INC_DIRS Include ${glfw_INCS}
    LIBS ${Vulkan_LIBRARIES} ${glfw_LIBS} ${PLATFORM_LIBS}
)
