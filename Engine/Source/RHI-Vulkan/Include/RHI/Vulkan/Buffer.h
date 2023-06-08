//
// Created by johnk on 2022/1/26.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <RHI/Buffer.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKBuffer : public Buffer {
    public:
        VKBuffer(VKDevice& device, const BufferCreateInfo& createInfo);
        ~VKBuffer();

        void* Map(MapMode mapMode, size_t offset, size_t length) override;
        void UnMap() override;
        BufferView* CreateBufferView(const BufferViewCreateInfo& createInfo) override;
        void Destroy() override;

        VkBuffer GetVkBuffer();
        BufferUsageFlags GetUsages();

    private:
        void CreateBuffer(const BufferCreateInfo& createInfo);
        void AllocateMemory(const BufferCreateInfo& createInfo);
        void DestroyBuffer();
        void FreeMemory();

        VKDevice& device;
        VkDeviceMemory vkDeviceMemory;
        VkBuffer vkBuffer;
        VmaAllocation allocation;
        BufferUsageFlags usages;
    };
}
