//
// Created by Administrator on 2021/1/9 0009.
//

#ifndef EXPLOSION_VULKAN_DEVICE_H
#define EXPLOSION_VULKAN_DEVICE_H

// TODO remove this
#define VK_VALIDATION_LAYER_ENABLED

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>

#include <vulkan/vulkan.h>
#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#endif

#include <driver/device.h>

namespace Explosion {
    class VulkanDevice : public Device {
    public:
        VulkanDevice();
        ~VulkanDevice() override;

    private:
        void Prepare();
        void CreateVkInstance();
        void PickVkPhysicalDevice();
        void CreateVkLogicalDevice();

        void DestroyVkInstance();
        void DestroyVkLogicalDevice();

#ifdef VK_VALIDATION_LAYER_ENABLED
        void CreateDebugUtils();
        void DestroyDebugUtils();
#endif

        // properties begin
        std::vector<const char*> vkExtensions;
#ifdef VK_VALIDATION_LAYER_ENABLED
        std::vector<const char*> vkLayers;
#endif

        VkInstance vkInstance = VK_NULL_HANDLE;

        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties {};
        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures {};
        uint32_t vkQueueFamilyIndex = 0;

        VkDevice vkLogicalDevice = VK_NULL_HANDLE;
        VkQueue vkQueue = VK_NULL_HANDLE;

#ifdef VK_VALIDATION_LAYER_ENABLED
        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
#endif
    };
}

#endif //EXPLOSION_VULKAN_DEVICE_H
