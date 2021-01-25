//
// Created by John Kindem on 2021/1/9 0009.
//

#ifndef EXPLOSION_VULKAN_CONTEXT_H
#define EXPLOSION_VULKAN_CONTEXT_H

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
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#include <driver/context.h>

namespace Explosion {
    class VulkanContext : public Context {
    public:
        VulkanContext();
        ~VulkanContext() override;

        const VkInstance& GetVkInstance();
        const VkPhysicalDevice& GetVkPhysicalDevice();
        const VkDevice& GetVkDevice();
        const VkQueue& GetVkQueue();

    private:
        void PrepareInstanceExtensions();
        void PrepareValidationLayers();
        void CreateVkInstance();
        void PickVkPhysicalDevice();
        void PickQueueFamilyIndex();
        void CreateVkLogicalDevice();

        void DestroyVkInstance();
        void DestroyVkLogicalDevice();

#ifdef VK_VALIDATION_LAYER_ENABLED
        void CreateDebugUtils();
        void DestroyDebugUtils();
#endif

        std::vector<const char*> vkInstanceExtensions;
#ifdef VK_VALIDATION_LAYER_ENABLED
        std::vector<const char*> vkValidationLayers;
#endif
        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties {};
        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures {};
        uint32_t vkQueueFamilyIndex = 0;
        VkDevice vkDevice = VK_NULL_HANDLE;
        VkQueue vkQueue = VK_NULL_HANDLE;
#ifdef VK_VALIDATION_LAYER_ENABLED
        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
#endif
    };
}

#endif //EXPLOSION_VULKAN_CONTEXT_H
