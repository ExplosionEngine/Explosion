//
// Created by johnk on 2022/1/26.
//

#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include <RHI/Buffer.h>
#include <Common/Utility.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanBuffer : public Buffer {
    public:
        NonCopyable(VulkanBuffer)
        VulkanBuffer(VulkanDevice& inDevice, const BufferCreateInfo& inCreateInfo);
        ~VulkanBuffer() override;

        void* Map(MapMode inMapMode, size_t inOffset, size_t inLength) override;
        void UnMap() override;
        Holder<BufferView> CreateBufferView(const BufferViewCreateInfo& inCreateInfo) override;
        void Destroy() override;

        VkBuffer GetNative();
        BufferUsageFlags GetUsages();

    private:
        void CreateNativeBuffer(const BufferCreateInfo& inCreateInfo);
        void TransitionToInitState(const BufferCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkBuffer nativeBuffer;
        VmaAllocation nativeAllocation;
        BufferUsageFlags usages;
    };
}
