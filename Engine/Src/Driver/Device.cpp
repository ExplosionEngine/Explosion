//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>
#include <functional>

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/Utils.h>
#include <Explosion/Driver/Platform.h>
#include <Explosion/Common/Logger.h>

#define VK_VALIDATION_LAYER_EXTENSION_NAME "VK_LAYER_KHRONOS_validation"

namespace {
    const std::vector<Explosion::RateRule<VkPhysicalDevice>> PHYSICAL_DEVICE_RATE_RULES = {
        [](const auto& device) -> uint32_t {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);

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
        }
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
}

namespace Explosion {
    Device::Device()
    {
        PrepareInstanceExtensions();
        PrepareLayers();
        CreateInstance();
#ifdef ENABLE_VALIDATION_LAYER
        CreateDebugUtils();
#endif
        PickPhysicalDevice();
        GetSelectedPhysicalDeviceProperties();
        FindQueueFamilyIndex();
        PrepareDeviceExtensions();
        CreateLogicalDevice();
        GetQueue();
        CreateCommandPool();
    }

    Device::~Device()
    {
        DestroyCommandPool();
        DestroyLogicalDevice();
#ifdef ENABLE_VALIDATION_LAYER
        DestroyDebugUtils();
#endif
        DestroyInstance();
    }

    void Device::PrepareInstanceExtensions()
    {
        instanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef ENABLE_VALIDATION_LAYER
        instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        auto platformExtensionNum = GetPlatformInstanceExtensionNum();
        auto platformExtensions = GetPlatformInstanceExtensions();
        instanceExtensions.insert(instanceExtensions.end(), platformExtensions, platformExtensions + platformExtensionNum);

        uint32_t propertiesCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertiesCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, properties.data());
        if (!CheckPropertySupport<VkExtensionProperties>(
            instanceExtensions, properties,
            [](const auto* name, const auto& prop) -> bool { return std::string(name) == prop.extensionName; })
        ) {
            throw std::runtime_error("there are some instance extension is not supported");
        }
    }

    void Device::PrepareDeviceExtensions()
    {
        deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        uint32_t propertiesCnt = 0;
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertiesCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertiesCnt);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &propertiesCnt, properties.data());
        if (!CheckPropertySupport<VkExtensionProperties>(
            deviceExtensions, properties,
            [](const auto* name, const auto& prop) -> bool { return std::string(name) == prop.extensionName; })
        ) {
            throw std::runtime_error("there are some device extension is not supported");
        }
    }

    const VkInstance& Device::GetVkInstance() const
    {
        return vkInstance;
    }

    const VkPhysicalDevice& Device::GetVkPhysicalDevice() const
    {
        return vkPhysicalDevice;
    }

    const VkDevice& Device::GetVkDevice() const
    {
        return vkDevice;
    }

    const VkQueue& Device::GetVkQueue() const
    {
        return vkQueue;
    }

    uint32_t Device::GetVkQueueFamilyIndex() const
    {
        return vkQueueFamilyIndex.value();
    }

    const VkCommandPool& Device::GetVkCommandPool() const
    {
        return vkCommandPool;
    }

    void Device::PrepareLayers()
    {
#ifdef ENABLE_VALIDATION_LAYER
        layers.emplace_back(VK_VALIDATION_LAYER_EXTENSION_NAME);
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
        createInfo.enabledExtensionCount = instanceExtensions.size();
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
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
        vkPhysicalDevice = Rate<VkPhysicalDevice>(devices, PHYSICAL_DEVICE_RATE_RULES)[0].second;
    }

    void Device::GetSelectedPhysicalDeviceProperties()
    {
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);
    }

    void Device::FindQueueFamilyIndex()
    {
        uint32_t queueFamilyCnt = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCnt, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCnt, queueFamilyProperties.data());

        for (uint32_t i = 0; i < queueFamilyCnt; i++) {
            if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                vkQueueFamilyIndex = i;
                break;
            }
        }
        if (!vkQueueFamilyIndex.has_value()) {
            throw std::runtime_error("found no queue family with graphics queue supported");
        }
    }

    void Device::CreateLogicalDevice()
    {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = vkQueueFamilyIndex.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
        deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        // NB: this field is ignored in high version of Vulkan, but in low version, code will not work will
        deviceCreateInfo.enabledLayerCount = 0;

        if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }
    }

    void Device::DestroyLogicalDevice()
    {
        vkDestroyDevice(vkDevice, nullptr);
    }

    void Device::GetQueue()
    {
        vkGetDeviceQueue(vkDevice, vkQueueFamilyIndex.value(), 0, &vkQueue);
    }

    void Device::CreateCommandPool()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo {};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = nullptr;
        commandPoolCreateInfo.flags = 0;
        commandPoolCreateInfo.queueFamilyIndex = vkQueueFamilyIndex.value();

        if (vkCreateCommandPool(vkDevice, &commandPoolCreateInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan command pool");
        }
    }

    void Device::DestroyCommandPool()
    {
        vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
    }
}
