//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_VULKANBUFFER_H
#define EXPLOSION_VULKANBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Enum.h>
#include <Explosion/RHI/Vulkan/VulkanUtils.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanBuffer {
    public:
        struct Config {
            uint32_t size;
            std::vector<BufferUsage> usages;
            std::vector<MemoryProperty> memoryProperties;
        };

        VulkanBuffer(VulkanDriver& driver, Config config);
        ~VulkanBuffer();
        uint32_t GetSize();
        const VkBuffer& GetVkBuffer();
        const VkDeviceMemory& GetVkDeviceMemory();
        void UpdateData(void* data);

    private:
        void DestroyBuffer();
        void CreateBuffer();

        void AllocateMemory();
        void FreeMemory();

        VulkanDriver& driver;
        VulkanDevice& device;
        Config config;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
        bool isDeviceLocal = false;
    };
}

#endif //EXPLOSION_VULKANBUFFER_H
