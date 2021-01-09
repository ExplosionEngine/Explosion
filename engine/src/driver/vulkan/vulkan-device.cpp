//
// Created by Administrator on 2021/1/9 0009.
//

#include <driver/vulkan/vulkan-device.h>

namespace {
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverityFlagBits,
        VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* pUserData) {
        std::cerr << "[ValidationLayer] " << callbackData->pMessage << std::endl;
        return VK_FALSE;
    }

#ifdef VK_VALIDATION_LAYER_ENABLED
    VkDebugUtilsMessengerCreateInfoEXT PopulateDebugUtilsMessengerCreateInfo() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
             | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;
        return createInfo;
    }
#endif

    void ValidateExtensions(
        const std::vector<const char*>& required,
        const std::vector<VkExtensionProperties>& available) {
        for (const auto& extension : required) {
            bool found = false;
            for (const auto& property : available) {
                std::string name = property.extensionName;
                if (extension == name) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw std::runtime_error(std::string("extension is not supported: ") + extension);
            }
        }
    }

    void ValidateValidationLayers(
        const std::vector<const char*>& required,
        const std::vector<VkLayerProperties>& available) {
        for (const auto& layer : required) {
            bool found = false;
            for (const auto& property : available) {
                std::string name = property.layerName;
                if (layer == name) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                throw std::runtime_error(std::string("layer is not supported: ") + layer);
            }
        }
    }

    std::vector<const char*> PrepareExtensions() {
        std::vector<const char*> required;
        required.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
        required.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_VALIDATION_LAYER_ENABLED
        required.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        uint32_t availableCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableCnt, nullptr);
        std::vector<VkExtensionProperties> available(availableCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableCnt, available.data());

        ValidateExtensions(required, available);
        return required;
    }

    std::vector<const char*> PrepareValidationLayers() {
        std::vector<const char*> required;
#ifdef VK_VALIDATION_LAYER_ENABLED
        required.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

        uint32_t availableCnt = 0;
        vkEnumerateInstanceLayerProperties(&availableCnt, nullptr);
        std::vector<VkLayerProperties> available(availableCnt);
        vkEnumerateInstanceLayerProperties(&availableCnt, available.data());

        ValidateValidationLayers(required, available);
        return required;
    }

    using RateRule = std::function<uint32_t(const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features)>;
    const std::vector<RateRule> RATE_RULES {
        { [](const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features) -> uint32_t {
            switch (properties.deviceType) {
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return 1000;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return 100;
                default:
                    return 1;
            }
        } }
    };

    std::vector<std::pair<VkPhysicalDevice, uint32_t>> RatePhysicalDevices(
        const std::vector<VkPhysicalDevice>& devices) {
        std::vector<std::pair<VkPhysicalDevice, uint32_t>> result;
        for (const auto& device : devices) {
            uint32_t score = 0;

            VkPhysicalDeviceProperties deviceProperties {};
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            VkPhysicalDeviceFeatures deviceFeatures {};
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            for (const auto& rule : RATE_RULES) {
                score += rule(deviceProperties, deviceFeatures);
            }
            result.emplace_back(std::make_pair(device, score));
        }
        return result;
    }

    uint32_t GetQueueFamilyIndex(const VkPhysicalDevice& device) {
        uint32_t queueFamilyCnt = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCnt, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCnt, queueFamilies.data());

        for (auto i = 0; i < queueFamilies.size(); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                // TODO judge if the graphics queue supports present
                return i;
            }
        }
        throw std::runtime_error("physical device has no queue families");
    }
}

namespace Explosion {
    VulkanDevice::VulkanDevice() {
        Prepare();
        CreateVkInstance();
#ifdef VK_VALIDATION_LAYER_ENABLED
        CreateDebugUtils();
#endif
        PickVkPhysicalDevice();
        CreateVkLogicalDevice();
    }

    VulkanDevice::~VulkanDevice() {
        DestroyVkLogicalDevice();
#ifdef VK_VALIDATION_LAYER_ENABLED
        DestroyDebugUtils();
#endif
        DestroyVkInstance();
    }

    void VulkanDevice::Prepare() {
        vkExtensions = PrepareExtensions();
        vkLayers = PrepareValidationLayers();
    }

    void VulkanDevice::CreateVkInstance() {
        VkApplicationInfo applicationInfo {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Explosion GameEngine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Explosion";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &applicationInfo;
        createInfo.enabledExtensionCount = vkExtensions.size();
        createInfo.ppEnabledExtensionNames = vkExtensions.data();
#ifdef VK_VALIDATION_LAYER_ENABLED
        auto debugUtilsMessengerCreateInfo = PopulateDebugUtilsMessengerCreateInfo();;
        createInfo.enabledLayerCount = vkLayers.size();
        createInfo.ppEnabledLayerNames = vkLayers.data();
        createInfo.pNext = &debugUtilsMessengerCreateInfo;
#else
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
#endif

        VkResult result = vkCreateInstance(&createInfo, nullptr, &vkInstance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance");
        }
    }

    void VulkanDevice::PickVkPhysicalDevice() {
        uint32_t deviceCnt = 0;
        vkEnumeratePhysicalDevices(vkInstance, &deviceCnt, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCnt);
        vkEnumeratePhysicalDevices(vkInstance, &deviceCnt, devices.data());
        if (deviceCnt == 0) {
            throw std::runtime_error("found no physical devices");
        }

        auto scores = RatePhysicalDevices(devices);
        vkPhysicalDevice = scores[0].first;
        vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkPhysicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &vkPhysicalDeviceFeatures);
        vkQueueFamilyIndex = GetQueueFamilyIndex(vkPhysicalDevice);
    }

    void VulkanDevice::CreateVkLogicalDevice() {
        float priority = 1.f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = GetQueueFamilyIndex(vkPhysicalDevice);
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &priority;

        VkDeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;
#ifdef VK_VALIDATION_LAYER_ENABLED
        deviceCreateInfo.enabledLayerCount = vkLayers.size();
        deviceCreateInfo.ppEnabledLayerNames = vkLayers.data();
#else
        deviceCreateInfo.enabledLayerCount = 0;
#endif

        VkResult result = vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkLogicalDevice);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }
        vkGetDeviceQueue(vkLogicalDevice, vkQueueFamilyIndex, 0, &vkQueue);
    }

    void VulkanDevice::DestroyVkInstance() {
        vkDestroyInstance(vkInstance, nullptr);
    }

    void VulkanDevice::DestroyVkLogicalDevice() {
        vkDestroyDevice(vkLogicalDevice, nullptr);
    }

#ifdef VK_VALIDATION_LAYER_ENABLED
    void VulkanDevice::CreateDebugUtils() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = PopulateDebugUtilsMessengerCreateInfo();

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func == nullptr) {
            throw std::runtime_error("failed to get func address: vkCreateDebugUtilsMessengerEXT");
        }
        VkResult result = func(vkInstance, &createInfo, nullptr, &vkDebugUtilsMessenger);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create debug utils messenger");
        }
    }

    void VulkanDevice::DestroyDebugUtils() {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func == nullptr) {
            throw std::runtime_error("failed to get func address: vkDestroyDebugUtilsMessengerEXT");
        }
        func(vkInstance, vkDebugUtilsMessenger, nullptr);
    }
#endif
}