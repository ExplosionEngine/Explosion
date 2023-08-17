//
// Created by johnk on 2023/8/7.
//

#include <RHI/Vulkan/VulkanRHIModule.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VulkanRHIModule::VulkanRHIModule() = default;

    VulkanRHIModule::~VulkanRHIModule() = default;

    Instance* VulkanRHIModule::GetRHIInstance() // NOLINT
    {
        return RHIGetInstance();
    }
}
