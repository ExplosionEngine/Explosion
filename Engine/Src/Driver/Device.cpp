//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>
#include <functional>

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/Utils.h>
#include <Explosion/Common/Logger.h>

#ifdef WIN32
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#include <vulkan/vulkan_macos.h>
#endif

namespace {
    using RateRule = std::function<uint32_t(const VkPhysicalDeviceProperties&, const VkPhysicalDeviceFeatures&)>;

    const std::vector<RateRule> RULES = {
        { [](const auto& properties, const auto& features) -> uint32_t {
            switch (properties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return 1000;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return 100;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return 50;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    return 1;
                default:
                    return 0;
            }
        } }
    };

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData
    ) {
        if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            return VK_FALSE;
        }
        Explosion::Logger::Error(std::string("validation layer: ") + callbackData->pMessage);
        return VK_FALSE;
    }

    VkDebugUtilsMessengerCreateInfoEXT GetDebugUtilsMessengerCreateInfo()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;
        return createInfo;
    }

    std::vector<std::pair<uint32_t, VkPhysicalDevice>> RatePhysicalDevices(const std::vector<VkPhysicalDevice>& devices)
    {
        std::vector<std::pair<uint32_t, VkPhysicalDevice>> result;
        for (auto& device : devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

            uint32_t scores = 0;
            for (const auto& rule : RULES) {
                scores += rule(properties, features);
            }
            result.emplace_back(std::make_pair(scores, device));
        }
        std::sort(result.begin(), result.end(), [](const auto& a, const auto& b) -> bool { return a.first > b.first; });
        return result;
    }
}

namespace Explosion {
    Device::Device()
    {
        PrepareExtensions();
        PrepareLayers();
        CreateInstance();
#ifdef ENABLE_VALIDATION_LAYER
        CreateDebugUtils();
#endif
        PickPhysicalDevice();
    }

    Device::~Device()
    {
#ifdef ENABLE_VALIDATION_LAYER
        DestroyDebugUtils();
#endif
        DestroyInstance();
    }

    void Device::PrepareExtensions()
    {
        extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef WIN32
        extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef TARGET_OS_MAC
        extensions.emplace_back("VK_EXT_metal_surface");
#endif
#ifdef ENABLE_VALIDATION_LAYER
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        uint32_t propertiesCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertiesCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, properties.data());
        if (!CheckPropertySupport<VkExtensionProperties>(
            extensions, properties,
            [](const auto* name, const auto& prop) -> bool { return std::string(name) == prop.extensionName; })
        ) {
            throw std::runtime_error("there are some extension is not supported");
        }
    }

    void Device::PrepareLayers()
    {
#ifdef ENABLE_VALIDATION_LAYER
        layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

        uint32_t layerCnt = 0;
        vkEnumerateInstanceLayerProperties(&layerCnt, nullptr);
        std::vector<VkLayerProperties> properties(layerCnt);
        vkEnumerateInstanceLayerProperties(&layerCnt, properties.data());
        if (!CheckPropertySupport<VkLayerProperties>(
            layers, properties,
            [](const auto* name, const auto& prop) -> bool { return std::string(name) == prop.layerName; }
        )) {
            throw std::runtime_error("there are some layers is not supported");
        }
    }

    void Device::CreateInstance()
    {
        VkApplicationInfo applicationInfo {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "ExplosionEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "ExplosionEngine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
#ifdef ENABLE_VALIDATION_LAYER
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = GetDebugUtilsMessengerCreateInfo();
        createInfo.pNext = &debugUtilsMessengerCreateInfo;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan instance");
        }
    }

    void Device::DestroyInstance()
    {
        vkDestroyInstance(vkInstance, nullptr);
    }

#ifdef ENABLE_VALIDATION_LAYER
    void Device::CreateDebugUtils()
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = GetDebugUtilsMessengerCreateInfo();
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func == nullptr) {
            throw std::runtime_error("failed to get function address of vkCreateDebugUtilsMessengerEXT");
        }
        if (func(vkInstance, &createInfo, nullptr, &vkDebugUtilsMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to create debug utils messenger");
        }
    }

    void Device::DestroyDebugUtils()
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func == nullptr) {
            throw std::runtime_error("failed to get function address of vkDestroyDebugUtilsMessengerEXT");
        }
        func(vkInstance, vkDebugUtilsMessenger, nullptr);
    }
#endif

    void Device::PickPhysicalDevice()
    {
        uint32_t deviceCnt = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCnt, nullptr);
        if (deviceCnt == 0) {
            throw std::runtime_error("there is no vulkan physical device available");
        }
        std::vector<VkPhysicalDevice> devices(deviceCnt);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCnt, devices.data());
        vkPhysicalDevice = RatePhysicalDevices(devices)[0].second;
    }
}
