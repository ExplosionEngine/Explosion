//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>

#include <Explosion/Driver/Device.h>
#include <Explosion/Driver/Utils.h>
#include <Explosion/Common/Logger.h>

#ifdef WIN32
#include <vulkan/vulkan_win32.h>
#endif

namespace {
    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData
    ) {
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
        PrepareExtensions();
        PrepareLayers();
        CreateInstance();
#ifdef ENABLE_VALIDATION_LAYER
        CreateDebugUtils();
#endif
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
#ifdef ENABLE_VALIDATION_LAYER
        extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        uint32_t propertiesCnt = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, nullptr);
        std::vector<VkExtensionProperties> properties(propertiesCnt);
        vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCnt, properties.data());
        if (!CheckExtensionSupported(extensions, properties)) {
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
        if (!CheckLayerSupported(layers, properties)) {
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
}
