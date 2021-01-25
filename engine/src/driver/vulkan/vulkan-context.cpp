//
// Created by Administrator on 2021/1/9 0009.
//

#include <optional>

#include <driver/vulkan/vulkan-context.h>

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

    template<typename A, typename B>
    using CompareFunc = std::function<bool(const A&, const B&)>;

    template<typename A, typename B>
    std::vector<A> ValidateVectorBContainsVectorA(
        const std::vector<A>& arrayA,
        const std::vector<B>& arrayB,
        const CompareFunc<A, B>& compareFunc
    ) {
        std::vector<A> except;
        for (const auto& a : arrayA) {
            bool found = false;
            for (const auto& b : arrayB) {
                if (compareFunc(a, b)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                except.emplace_back(a);
            }
        }
        return except;
    }

    void ValidateExtensions(
        const std::vector<const char*>& required,
        const std::vector<VkExtensionProperties>& available) {
        std::vector<const char*> except = ValidateVectorBContainsVectorA<const char*, VkExtensionProperties>(
            required,
            available,
            [](const auto& a, const auto& b) -> bool { return std::string(a) == b.extensionName; }
        );
        for (const auto& extension : except) {
            std::cout << (std::string("[explosion] extension: ") + extension + " is not supported") << std::endl;
        }
        throw std::runtime_error("[explosion] failed to check all extensions");
    }

    void ValidateValidationLayers(
        const std::vector<const char*>& required,
        const std::vector<VkLayerProperties>& available) {
        std::vector<const char*> except = ValidateVectorBContainsVectorA<const char*, VkLayerProperties>(
            required,
            available,
            [](const auto& a, const auto& b) -> bool { return std::string(a) == b.layerName; }
        );
        for (const auto& layer : except) {
            std::cout << (std::string("[explosion] layer: ") + layer + " is not supported") << std::endl;
        }
        throw std::runtime_error("[explosion] failed to check all validation layer");
    }

    using RateRule = std::function<uint32_t(const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features)>;
    const std::vector<RateRule> RATE_RULES {
        { [](const auto& properties, const auto& features) -> uint32_t {
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
}

namespace Explosion {
    VulkanContext::VulkanContext() {
        PrepareInstanceExtensions();
        PrepareValidationLayers();
        CreateVkInstance();
#ifdef VK_VALIDATION_LAYER_ENABLED
        CreateDebugUtils();
#endif
        PickVkPhysicalDevice();
        PickQueueFamilyIndex();
        CreateVkLogicalDevice();
    }

    VulkanContext::~VulkanContext() {
        DestroyVkLogicalDevice();
#ifdef VK_VALIDATION_LAYER_ENABLED
        DestroyDebugUtils();
#endif
        DestroyVkInstance();
    }

    const VkInstance& VulkanContext::GetVkInstance() {
        return vkInstance;
    }

    const VkPhysicalDevice & VulkanContext::GetVkPhysicalDevice() {
        return vkPhysicalDevice;
    }

    const VkDevice& VulkanContext::GetVkDevice() {
        return vkDevice;
    }

    const VkQueue& VulkanContext::GetVkQueue() {
        return vkQueue;
    }

    void VulkanContext::PrepareInstanceExtensions() {
        vkInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
        vkInstanceExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_VALIDATION_LAYER_ENABLED
        vkInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        uint32_t availableCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableCnt, nullptr);
        std::vector<VkExtensionProperties> available(availableCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableCnt, available.data());

        ValidateExtensions(vkInstanceExtensions, available);
    }

    void VulkanContext::PrepareValidationLayers() {
#ifdef VK_VALIDATION_LAYER_ENABLED
        vkValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

        uint32_t availableCnt = 0;
        vkEnumerateInstanceLayerProperties(&availableCnt, nullptr);
        std::vector<VkLayerProperties> available(availableCnt);
        vkEnumerateInstanceLayerProperties(&availableCnt, available.data());

        ValidateValidationLayers(vkValidationLayers, available);
    }

    void VulkanContext::CreateVkInstance() {
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
        createInfo.enabledExtensionCount = vkInstanceExtensions.size();
        createInfo.ppEnabledExtensionNames = vkInstanceExtensions.data();
#ifdef VK_VALIDATION_LAYER_ENABLED
        auto debugUtilsMessengerCreateInfo = PopulateDebugUtilsMessengerCreateInfo();;
        createInfo.enabledLayerCount = vkValidationLayers.size();
        createInfo.ppEnabledLayerNames = vkValidationLayers.data();
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

    void VulkanContext::PickVkPhysicalDevice() {
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
    }

    void VulkanContext::PickQueueFamilyIndex()
    {
        uint32_t queueFamilyCnt = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCnt, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCnt);
        vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCnt, queueFamilies.data());

        std::optional<uint32_t> idx;
        for (auto i = 0; i < queueFamilies.size(); i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                idx = i;
            }
        }
        if (!idx.has_value()) {
            throw std::runtime_error("[explosion] found no suitable queue family");
        }
        vkQueueFamilyIndex = idx.value();
    }

    void VulkanContext::CreateVkLogicalDevice() {
        float priority = 1.f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = vkQueueFamilyIndex;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &priority;

        VkDeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &vkPhysicalDeviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;
#ifdef VK_VALIDATION_LAYER_ENABLED
        deviceCreateInfo.enabledLayerCount = vkValidationLayers.size();
        deviceCreateInfo.ppEnabledLayerNames = vkValidationLayers.data();
#else
        deviceCreateInfo.enabledLayerCount = 0;
#endif

        VkResult result = vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device");
        }
        vkGetDeviceQueue(vkDevice, vkQueueFamilyIndex, 0, &vkQueue);
    }

    void VulkanContext::DestroyVkInstance() {
        vkDestroyInstance(vkInstance, nullptr);
    }

    void VulkanContext::DestroyVkLogicalDevice() {
        vkDestroyDevice(vkDevice, nullptr);
    }

#ifdef VK_VALIDATION_LAYER_ENABLED
    void VulkanContext::CreateDebugUtils() {
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

    void VulkanContext::DestroyDebugUtils() {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
        if (func == nullptr) {
            throw std::runtime_error("failed to get func address: vkDestroyDebugUtilsMessengerEXT");
        }
        func(vkInstance, vkDebugUtilsMessenger, nullptr);
    }
#endif
}