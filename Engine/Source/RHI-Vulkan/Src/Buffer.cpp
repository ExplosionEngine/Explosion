//
// Created by johnk on 2022/1/26.
//

#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/BufferView.h>

namespace RHI::Vulkan {
    static VkBufferUsageFlags GetVkResourceStates(BufferUsageFlags bufferUsages)
    {
        static std::unordered_map<BufferUsageBits, VkBufferUsageFlags> rules = {
            { BufferUsageBits::copySrc, VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
            { BufferUsageBits::copyDst, VK_BUFFER_USAGE_TRANSFER_DST_BIT },
            { BufferUsageBits::index,    VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
            { BufferUsageBits::vertex,   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
            { BufferUsageBits::uniform,  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
            { BufferUsageBits::storage,  VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
            { BufferUsageBits::indirect, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT },
        };

        VkBufferUsageFlags result = {};
        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    VKBuffer::VKBuffer(VKDevice& d, const BufferCreateInfo& createInfo) : Buffer(createInfo), device(d), usages(createInfo.usages)
    {
        CreateBuffer(createInfo);
    }

    VKBuffer::~VKBuffer()
    {
        vmaDestroyBuffer(device.GetVmaAllocator(), vkBuffer, allocation);
    }

    void* VKBuffer::Map(MapMode mapMode, size_t offset, size_t length)
    {
        void* data;
        Assert(vmaMapMemory(device.GetVmaAllocator(), allocation, &data) == VK_SUCCESS);
        return data;
    }

    void VKBuffer::UnMap()
    {
        vmaUnmapMemory(device.GetVmaAllocator(), allocation);
    }

    BufferView* VKBuffer::CreateBufferView(const BufferViewCreateInfo& createInfo)
    {
        return new VKBufferView(*this, createInfo);
    }

    void VKBuffer::Destroy()
    {
        delete this;
    }

    void VKBuffer::CreateBuffer(const BufferCreateInfo& createInfo)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = GetVkResourceStates(createInfo.usages);
        bufferInfo.size = createInfo.size;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
        if (createInfo.usages | BufferUsageBits::mapWrite) {
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }

        Assert(vmaCreateBuffer(device.GetVmaAllocator(), &bufferInfo, &allocInfo, &vkBuffer, &allocation, nullptr) == VK_SUCCESS);

#if BUILD_CONFIG_DEBUG
        vmaSetAllocationName(device.GetVmaAllocator(), allocation, createInfo.debugName.c_str());
#endif
    }

    VkBuffer VKBuffer::GetVkBuffer()
    {
        return vkBuffer;
    }

    BufferUsageFlags VKBuffer::GetUsages()
    {
        return usages;
    }
}
