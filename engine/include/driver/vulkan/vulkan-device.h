//
// Created by Administrator on 2021/1/9 0009.
//

#ifndef EXPLOSION_VULKAN_DEVICE_H
#define EXPLOSION_VULKAN_DEVICE_H

// TODO remove this
#define VK_VALIDATION_LAYER_ENABLED

#include <string>
#include <vector>

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
        void CreateVkInstance();
        void PickVkPhysicalDevice();
        void CreateVkLogicalDevice();
        void FetchVkQueue();

        void DestroyVkInstance();
        void DestroyVkLogicalDevice();

        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice vkLogicalDevice = VK_NULL_HANDLE;
        VkQueue vkQueue = VK_NULL_HANDLE;

#ifdef VK_VALIDATION_LAYER_ENABLED
        void CreateDebugUtils();
        void DestroyDebugUtils();

        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
#endif
    };
}

#endif //EXPLOSION_VULKAN_DEVICE_H
