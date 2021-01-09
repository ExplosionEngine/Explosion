//
// Created by Administrator on 2021/1/9 0009.
//

#include <driver/vulkan/vulkan-device.h>

namespace {
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

        auto extensions = PrepareExtensions();
        VkInstanceCreateInfo instanceCreateInfo {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = extensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = 0;

        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance");
        }
    }
}