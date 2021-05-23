//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_VULKANDEVICE_H
#define EXPLOSION_VULKANDEVICE_H

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Device.h>

namespace Explosion::RHI {
    class VulkanDriver;

    class VulkanDevice : public Device {
    public:
        explicit VulkanDevice(VulkanDriver& driver);
        ~VulkanDevice() override;

        const VkInstance& GetVkInstance();
        const VkPhysicalDevice& GetVkPhysicalDevice();
        const VkDevice& GetVkDevice();
        const VkQueue& GetVkQueue();
        uint32_t GetVkQueueFamilyIndex();
        const VkCommandPool& GetVkCommandPool();
        const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties();

    private:
#ifdef ENABLE_VALIDATION_LAYER
        void CreateDebugUtils();
        void DestroyDebugUtils();
        VkDebugUtilsMessengerEXT vkDebugUtilsMessenger = VK_NULL_HANDLE;
#endif

        void PrepareInstanceExtensions();
        void PrepareDeviceExtensions();
        void PrepareLayers();

        void CreateInstance();
        void DestroyInstance();

        void PickPhysicalDevice();
        void GetSelectedPhysicalDeviceProperties();
        void FindQueueFamilyIndex();

        void CreateLogicalDevice();
        void DestroyLogicalDevice();
        void GetQueue();

        void CreateCommandPool();
        void DestroyCommandPool();

        void FetchPhysicalDeviceMemoryProperties();

        VulkanDriver& driver;
        std::vector<const char*> instanceExtensions {};
        std::vector<const char*> deviceExtensions {};
        std::vector<const char*> layers {};
        VkInstance vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties{};
        VkPhysicalDeviceFeatures  vkPhysicalDeviceFeatures{};
        std::optional<uint32_t> vkQueueFamilyIndex;
        VkDevice vkDevice = VK_NULL_HANDLE;
        VkQueue vkQueue = VK_NULL_HANDLE;
        VkCommandPool vkCommandPool = VK_NULL_HANDLE;
        VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties{};
    };
}

#endif //EXPLOSION_VULKANDEVICE_H
