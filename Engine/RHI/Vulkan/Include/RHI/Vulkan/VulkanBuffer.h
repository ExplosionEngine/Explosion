//
// Created by John Kindem on 2021/4/28.
//

#ifndef EXPLOSION_VULKANBUFFER_H
#define EXPLOSION_VULKANBUFFER_H

#include <cstdint>

#include <vulkan/vulkan.h>

#include <RHI/Buffer.h>
#include <RHI/Vulkan/Api.h>
#include <RHI/Vulkan/VulkanUtils.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class RHI_VULKAN_API VulkanBuffer : public Buffer {
    public:
        VulkanBuffer(VulkanDriver& driver, Config config);
        ~VulkanBuffer() override;
        uint32_t GetSize() override;
        void UpdateData(void* data) override;

        const VkBuffer& GetVkBuffer();
        const VkDeviceMemory& GetVkDeviceMemory();

    private:
        void DestroyBuffer();
        void CreateBuffer();

        void AllocateMemory();
        void FreeMemory();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkBuffer vkBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vkDeviceMemory = VK_NULL_HANDLE;
    };
}

#endif //EXPLOSION_VULKANBUFFER_H
