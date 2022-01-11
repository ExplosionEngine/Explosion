//
// Created by johnk on 11/1/2022.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VKInstance::VKInstance() : Instance()
    {
        CreateVKInstance();
    }

    VKInstance::~VKInstance()
    {
        DestroyVKInstance();
    }

    RHIType VKInstance::GetRHIType()
    {
        return RHIType::VULKAN;
    }

    void VKInstance::CreateVKInstance()
    {
        vk::ApplicationInfo applicationInfo;
        applicationInfo.pApplicationName = "Explosion GameEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Explosion GameEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        vk::InstanceCreateInfo createInfo;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        vk::Result result = vk::createInstance(&createInfo, nullptr, &vkInstance);
        if (result != vk::Result::eSuccess) {
            throw VKException("failed to create vulkan instance");
        }
    }

    void VKInstance::DestroyVKInstance()
    {
        vkInstance.destroy();
    }
}

extern "C" {
    RHI_VULKAN_API RHI::Instance* RHICreateInstance()
    {
        static RHI::Vulkan::VKInstance instance;
        return &instance;
    }
}
