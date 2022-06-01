//
// Created by Zach Lee on 2022/5/14.
//
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Api.h>

extern "C" {
RHI_VULKAN_API RHI::Instance* RHICreateInstance()
{
    static RHI::Vulkan::VKInstance instance;
    return &instance;
}
}
