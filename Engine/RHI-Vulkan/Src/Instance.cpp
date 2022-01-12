//
// Created by johnk on 11/1/2022.
//

#include <Common/Logger.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>

namespace RHI::Vulkan {
    static auto& GLogger = Common::Logger::Singleton().FindOrCreateDelegator("RHI-Vulkan");

#if BUILD_CONFIG_DEBUG
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            GLogger.Warning(callbackData->pMessage);
        } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            GLogger.Error(callbackData->pMessage);
        }
        return VK_FALSE;
    }
#endif

    VKInstance::VKInstance() : Instance()
    {
#if BUILD_CONFIG_DEBUG
        PrepareLayers();
#endif
        PrepareExtensions();
        CreateVKInstance();
        PrepareDispatch();
#if BUILD_CONFIG_DEBUG
        CreateDebugMessenger();
#endif
        EnumeratePhysicalDevices();
    }

    VKInstance::~VKInstance()
    {
#if BUILD_CONFIG_DEBUG
        DestroyDebugMessenger();
#endif
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
            VK_KHR_PLATFORM_SURFACE_EXTENSION_NAME,
#endif
#if BUILD_CONFIG_DEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
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

    void VKInstance::PrepareDispatch()
    {
#if BUILD_CONFIG_DEBUG
        vkDispatch.vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkInstance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
        vkDispatch.vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkInstance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
#endif
    }

    uint32_t VKInstance::GetGpuNum()
    {
        return gpus.size();
    }

    Gpu* VKInstance::GetGpu(uint32_t index)
    {
        return gpus[index].get();
    }

    void VKInstance::EnumeratePhysicalDevices()
    {
        uint32_t count = 0;
        vkInstance.enumeratePhysicalDevices(&count, nullptr);
        vkPhysicalDevices.resize(count);
        vkInstance.enumeratePhysicalDevices(&count, vkPhysicalDevices.data());

        gpus.resize(count);
        for (uint32_t i = 0; i < count; i++) {
            gpus[i] = std::make_unique<VKGpu>(vkPhysicalDevices[i]);
        }
    }

#if BUILD_CONFIG_DEBUG
    void VKInstance::CreateDebugMessenger()
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo;
        createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        createInfo.pfnUserCallback = DebugCallback;

        vk::Result result = vkInstance.createDebugUtilsMessengerEXT(&createInfo, nullptr, &vkDebugMessenger, vkDispatch);
        if (result != vk::Result::eSuccess) {
            throw VKException("failed to create vulkan debug messenger");
        }
    }

    void VKInstance::DestroyDebugMessenger()
    {
        vkInstance.destroyDebugUtilsMessengerEXT(vkDebugMessenger, nullptr, vkDispatch);
    }
#endif
}

extern "C" {
    RHI_VULKAN_API RHI::Instance* RHICreateInstance()
    {
        static RHI::Vulkan::VKInstance instance;
        return &instance;
    }
}
