//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_DEVICE_H
#define EXPLOSION_DEVICE_H

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/GpuRes.h>

namespace Explosion {
    class Device : public GpuRes {
    public:
        explicit Device(Driver& driver);
        ~Device() override;

        const VkInstance& GetVkInstance() const;
        const VkPhysicalDevice& GetVkPhysicalDevice() const;
        const VkDevice& GetVkDevice() const;
        const VkQueue& GetVkQueue() const;
        uint32_t GetVkQueueFamilyIndex() const;
        const VkCommandPool& GetVkCommandPool() const;
        const VkPhysicalDeviceMemoryProperties& GetVkPhysicalDeviceMemoryProperties() const;

    protected:
        void OnCreate() override;
        void OnDestroy() override;

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

#endif //EXPLOSION_DEVICE_H
