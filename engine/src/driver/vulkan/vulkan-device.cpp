//
// Created by Administrator on 2021/1/9 0009.
//

#include <driver/vulkan/vulkan-device.h>

namespace {
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
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

    void ValidateExtensions(std::vector<const char*> required, std::vector<VkExtensionProperties> available) {
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

    void ValidateValidationLayers(std::vector<const char*> required, std::vector<VkLayerProperties> available) {
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
}

namespace Explosion {
    VulkanDevice::VulkanDevice() {
        CreateVkInstance();
#ifdef VK_VALIDATION_LAYER_ENABLED
        CreateDebugUtils();
#endif
        PickVkPhysicalDevice();
        CreateVkLogicalDevice();
        FetchVkQueue();
    }

    VulkanDevice::~VulkanDevice() {
        DestroyVkLogicalDevice();
#ifdef VK_VALIDATION_LAYER_ENABLED
        DestroyDebugUtils();
#endif
        DestroyVkInstance();
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
        auto extensions = PrepareExtensions();
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
#ifdef VK_VALIDATION_LAYER_ENABLED
        auto layers = PrepareValidationLayers();
        auto debugUtilsMessengerCreateInfo = PopulateDebugUtilsMessengerCreateInfo();;
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
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