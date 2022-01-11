//
// Created by johnk on 11/1/2022.
//

#if PLATFORM_WINDOWS
#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#endif

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VKInstance::VKInstance() : Instance()
    {
        PrepareExtensions();
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

    void VKInstance::PrepareExtensions()
    {
        static std::vector<const char*> requiredExtensionNames = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#endif
        };

        uint32_t supportedExtensionCount = 0;
        vk::enumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<vk::ExtensionProperties> supportedExtensions(supportedExtensionCount);
        vk::enumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

        for (auto&& requiredExtensionName : requiredExtensionNames) {
            auto iter = std::find_if(
                supportedExtensions.begin(),
                supportedExtensions.end(),
                [&requiredExtensionName](const auto& elem) -> bool { return std::string(requiredExtensionName) == elem.extensionName; }
            );
            if (iter == supportedExtensions.end()) {
                continue;
            }
            vkEnabledExtensionNames.emplace_back(requiredExtensionName);
        }
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
        createInfo.enabledExtensionCount = vkEnabledExtensionNames.size();
        createInfo.ppEnabledExtensionNames = vkEnabledExtensionNames.data();
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
