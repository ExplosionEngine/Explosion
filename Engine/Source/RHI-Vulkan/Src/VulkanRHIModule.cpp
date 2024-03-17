//
// Created by johnk on 2023/8/7.
//

#include <RHI/Vulkan/VulkanRHIModule.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VulkanRHIModule::VulkanRHIModule() = default;

    VulkanRHIModule::~VulkanRHIModule() = default;

    void VulkanRHIModule::OnLoad()
    {
        gInstance = new VKInstance();
    }

    void VulkanRHIModule::OnUnload()
    {
        delete gInstance;
    }

    Instance* VulkanRHIModule::GetRHIInstance() // NOLINT
    {
        return gInstance;
    }
}

IMPLEMENT_MODULE(RHI_VULKAN_API, RHI::Vulkan::VulkanRHIModule);
