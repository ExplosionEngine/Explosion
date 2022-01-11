//
// Created by johnk on 11/1/2022.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>

namespace RHI::Vulkan {
    VKInstance::VKInstance() : Instance()
    {
#if BUILD_CONFIG_DEBUG
        PrepareLayers();
#endif

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

#if BUILD_CONFIG_DEBUG
    void VKInstance::PrepareLayers()
    {
        static std::vector<const char*> requiredLayerNames = {
            VK_KHRONOS_VALIDATION_LAYER_NAME
        };

        uint32_t supportedLayerCount = 0;
        vk::enumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        std::vector<vk::LayerProperties> supportedLayers(supportedLayerCount);
        vk::enumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

        for (auto&& requiredLayerName : requiredLayerNames) {
            auto iter = std::find_if(
                supportedLayers.begin(),
                supportedLayers.end(),
                [&requiredLayerName](const auto& elem) -> bool { return std::string(requiredLayerName) == elem.layerName; }
            );
            if (iter == supportedLayers.end()) {
                continue;
            }
            vkEnabledLayerNames.emplace_back(requiredLayerName);
        }
    }
#endif

    void VKInstance::PrepareExtensions()
    {
        static std::vector<const char*> requiredExtensionNames = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if PLATFORM_WINDOWS
            VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME
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
#if BUILD_CONFIG_DEBUG
        createInfo.enabledLayerCount = vkEnabledLayerNames.size();
        createInfo.ppEnabledLayerNames = vkEnabledLayerNames.data();
#endif

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
