//
// Created by johnk on 11/1/2022.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/Gpu.h>

namespace RHI::Vulkan {
    RHI::Instance* gInstance = nullptr;

#if BUILD_CONFIG_DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData)
    {
        std::cerr << callbackData->pMessage << std::endl;
        return VK_FALSE;
    }
#endif

    VulkanInstance::VulkanInstance() : Instance()
    {
#if BUILD_CONFIG_DEBUG
        PrepareLayers();
#endif
        PrepareExtensions();
        CreateNativeInstance();
        PrepareDynamicFuncPointers();
#if BUILD_CONFIG_DEBUG
        CreateDebugMessenger();
#endif
        EnumeratePhysicalDevices();
    }

    VulkanInstance::~VulkanInstance()
    {
#if BUILD_CONFIG_DEBUG
        DestroyDebugMessenger();
#endif
        DestroyNativeInstance();
    }

    RHIType VulkanInstance::GetRHIType()
    {
        return RHIType::vulkan;
    }

#if BUILD_CONFIG_DEBUG
    void VulkanInstance::PrepareLayers()
    {
        static std::vector<const char*> requiredLayerNames = {
            VK_KHRONOS_VALIDATION_LAYER_NAME
        };

        uint32_t supportedLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

        for (auto&& requiredLayerName : requiredLayerNames) {
            auto iter = std::find_if(
                supportedLayers.begin(),
                supportedLayers.end(),
                [&requiredLayerName](const auto& elem) -> bool { return std::string(requiredLayerName) == elem.layerName; }
            );
            if (iter == supportedLayers.end()) {
                QuickFailWithReason("not found required vulkan layers");
            }
            vkEnabledLayerNames.emplace_back(requiredLayerName);
        }
    }
#endif

    void VulkanInstance::PrepareExtensions()
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
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

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

    void VulkanInstance::CreateNativeInstance()
    {
        VkApplicationInfo applicationInfo = {};
        applicationInfo.pApplicationName = "Explosion GameEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Explosion GameEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = vkEnabledExtensionNames.size();
        createInfo.ppEnabledExtensionNames = vkEnabledExtensionNames.data();
#if BUILD_CONFIG_DEBUG
        createInfo.enabledLayerCount = vkEnabledLayerNames.size();
        createInfo.ppEnabledLayerNames = vkEnabledLayerNames.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
        PopulateDebugMessengerCreateInfo(debugCreateInfo);

        createInfo.pNext = &debugCreateInfo;
#endif
#if PLATFORM_MACOS
        createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        auto result = vkCreateInstance(&createInfo, nullptr, &nativeInstance);
        Assert(result == VK_SUCCESS);
    }

    void VulkanInstance::DestroyNativeInstance()
    {
        vkDestroyInstance(nativeInstance, nullptr);
    }

    void VulkanInstance::PrepareDynamicFuncPointers()
    {
#if BUILD_CONFIG_DEBUG
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(nativeInstance, "vkCreateDebugUtilsMessengerEXT"));
        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(nativeInstance, "vkDestroyDebugUtilsMessengerEXT"));
        pfnVkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(nativeInstance, "vkSetDebugUtilsObjectNameEXT"));
        Assert(pfnVkCreateDebugUtilsMessengerEXT != nullptr && pfnVkDestroyDebugUtilsMessengerEXT != nullptr && pfnVkSetDebugUtilsObjectNameEXT != nullptr);
#endif
        pfnVkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetInstanceProcAddr(nativeInstance, "vkCmdBeginRenderingKHR"));
        pfnVkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetInstanceProcAddr(nativeInstance, "vkCmdEndRenderingKHR"));
        Assert(pfnVkCmdBeginRenderingKHR != nullptr && pfnVkCmdEndRenderingKHR != nullptr);
    }

    uint32_t VulkanInstance::GetGpuNum()
    {
        return gpus.size();
    }

    Gpu* VulkanInstance::GetGpu(uint32_t inIndex)
    {
        return gpus[inIndex].Get();
    }

    VkInstance VulkanInstance::GetNative() const
    {
        return nativeInstance;
    }

    void VulkanInstance::EnumeratePhysicalDevices()
    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(nativeInstance, &count, nullptr);
        nativePhysicalDevices.resize(count);
        vkEnumeratePhysicalDevices(nativeInstance, &count, nativePhysicalDevices.data());

        gpus.resize(count);
        for (uint32_t i = 0; i < count; i++) {
            gpus[i] = new VulkanGpu(*this, nativePhysicalDevices[i]);
        }
    }

#if BUILD_CONFIG_DEBUG
    void VulkanInstance::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& inCreateInfo)
    {
        inCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        inCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        inCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        inCreateInfo.pfnUserCallback = DebugCallback;
        inCreateInfo.pNext = nullptr;
        inCreateInfo.flags = 0;
    }

    void VulkanInstance::CreateDebugMessenger()
    {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        PopulateDebugMessengerCreateInfo(debugCreateInfo);

        Assert(pfnVkCreateDebugUtilsMessengerEXT(nativeInstance, &debugCreateInfo, nullptr, &nativeDebugMessenger) == VK_SUCCESS);
    }

    void VulkanInstance::DestroyDebugMessenger()
    {
        pfnVkDestroyDebugUtilsMessengerEXT(nativeInstance, nativeDebugMessenger, nullptr);
    }
#endif

    void VulkanInstance::Destroy()
    {
        delete this;
    }
}
