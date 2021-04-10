//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DEVICE_H
#define EXPLOSION_DEVICE_H

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Device {
    public:
        Device();
        ~Device();

        const VkInstance& GetVkInstance() const;
        const VkPhysicalDevice& GetVkPhysicalDevice() const;
        const VkDevice& GetVkDevice() const;
        uint32_t GetVkQueueFamilyIndex() const;

    private:
#ifdef ENABLE_VALIDATION_LAYER
        void CreateDebugUtils();
        void DestroyDebugUtils();
        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
#endif

        void PrepareExtensions();
        void PrepareLayers();

        void CreateInstance();
        void DestroyInstance();

        void PickPhysicalDevice();
        void GetSelectedPhysicalDeviceProperties();
        void FindQueueFamilyIndex();

        void CreateLogicalDevice();
        void DestroyLogicalDevice();

        std::vector<const char*> extensions {};
        std::vector<const char*> layers {};
        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures  vkPhysicalDeviceFeatures;
        std::optional<uint32_t> vkQueueFamilyIndex;
        VkDevice vkDevice = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_DEVICE_H
