//
// Created by johnk on 11/1/2022.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>

namespace RHI::Vulkan {
#if BUILD_CONFIG_DEBUG
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        std::cerr << callbackData->pMessage << std::endl;
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
        (void)vk::enumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        std::vector<vk::LayerProperties> supportedLayers(supportedLayerCount);
        (void)vk::enumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

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
            "VK_KHR_win32_surface",
#elif PLATFORM_MACOS
            "VK_MVK_macos_surface",
            "VK_EXT_metal_surface",
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
#endif
#if BUILD_CONFIG_DEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
        };

        uint32_t supportedExtensionCount = 0;
        (void)vk::enumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<vk::ExtensionProperties> supportedExtensions(supportedExtensionCount);
        (void)vk::enumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

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

        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        debugCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        debugCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                      | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                      | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debugCreateInfo.pfnUserCallback = DebugCallback;

        createInfo.pNext = &debugCreateInfo;
#endif
#if PLATFORM_MACOS
        createInfo.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
#endif

        vk::Result result = vk::createInstance(&createInfo, nullptr, &vkInstance);
        Assert(result == vk::Result::eSuccess);
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
        vkDispatch.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkInstance.getProcAddr("vkCmdBeginRenderingKHR"));
        vkDispatch.vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkInstance.getProcAddr("vkCmdEndRenderingKHR"));
    }

    uint32_t VKInstance::GetGpuNum()
    {
        return gpus.size();
    }

    Gpu* VKInstance::GetGpu(uint32_t index)
    {
        return gpus[index].get();
    }

    vk::Instance VKInstance::GetVkInstance() const
    {
        return vkInstance;
    }

    vk::DispatchLoaderDynamic VKInstance::GetVkDispatch() const
    {
        return vkDispatch;
    }

    void VKInstance::EnumeratePhysicalDevices()
    {
        uint32_t count = 0;
        (void)vkInstance.enumeratePhysicalDevices(&count, nullptr);
        vkPhysicalDevices.resize(count);
        (void)vkInstance.enumeratePhysicalDevices(&count, vkPhysicalDevices.data());

        gpus.resize(count);
        for (uint32_t i = 0; i < count; i++) {
            gpus[i] = std::make_unique<VKGpu>(*this, vkPhysicalDevices[i]);
        }
    }

#if BUILD_CONFIG_DEBUG
    void VKInstance::CreateDebugMessenger()
    {
        vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        debugCreateInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        debugCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                      | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                                      | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debugCreateInfo.pfnUserCallback = DebugCallback;

        Assert(vkInstance.createDebugUtilsMessengerEXT(&debugCreateInfo, nullptr, &vkDebugMessenger, vkDispatch) == vk::Result::eSuccess);
    }

    void VKInstance::DestroyDebugMessenger()
    {
        vkInstance.destroyDebugUtilsMessengerEXT(vkDebugMessenger, nullptr, vkDispatch);
    }
#endif

    void VKInstance::Destroy()
    {
        delete this;
    }
}

extern "C" {
    RHI::Instance* RHIGetInstance()
    {
        static RHI::Vulkan::VKInstance instance;
        return &instance;
    }
}