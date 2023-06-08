//
// Created by johnk on 2022/1/26.
//

#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/BufferView.h>

namespace RHI::Vulkan {
    static VkMemoryPropertyFlags GetVkMemoryType(BufferUsageFlags bufferUsages)
    {
        static std::unordered_map<BufferUsageFlags, VkMemoryPropertyFlags> rules = {
            { BufferUsageBits::mapWrite | BufferUsageBits::copySrc, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT },
            { BufferUsageBits::mapRead | BufferUsageBits::copyDst, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT },
            { BufferUsageBits::uniform | BufferUsageBits::mapWrite, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT }
            // TODO check other conditions ?
        };
        static VkMemoryPropertyFlags fallback = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                return rule.second;
            }
        }
        return fallback;
    }


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
        AllocateMemory(createInfo);
    }

    VKBuffer::~VKBuffer()
    {
        FreeMemory();
        DestroyBuffer();
    }

    void* VKBuffer::Map(MapMode mapMode, size_t offset, size_t length)
    {
        void* data;
        Assert(vkMapMemory(device.GetVkDevice(), vkDeviceMemory, offset, length, {}, &data) == VK_SUCCESS);
        return data;
    }

    void VKBuffer::UnMap()
    {
        vkUnmapMemory(device.GetVkDevice(), vkDeviceMemory);
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
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = GetVkResourceStates(createInfo.usages);
        bufferInfo.size = createInfo.size;

        Assert(vkCreateBuffer(device.GetVkDevice(), &bufferInfo, nullptr, &vkBuffer) == VK_SUCCESS);
    }

    void VKBuffer::AllocateMemory(const BufferCreateInfo& createInfo)
    {
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(device.GetVkDevice(), vkBuffer, &memoryRequirements);

        VkMemoryAllocateInfo memoryInfo = {};
        memoryInfo.allocationSize = memoryRequirements.size;
        memoryInfo.memoryTypeIndex = device.GetGpu().FindMemoryType(memoryRequirements.memoryTypeBits, GetVkMemoryType(createInfo.usages));

        Assert(vkAllocateMemory(device.GetVkDevice(), &memoryInfo, nullptr, &vkDeviceMemory) == VK_SUCCESS);

        vkBindBufferMemory(device.GetVkDevice(), vkBuffer, vkDeviceMemory, 0);
    }

    void VKBuffer::DestroyBuffer()
    {
        if (vkBuffer) {
            vkDestroyBuffer(device.GetVkDevice(), vkBuffer, nullptr);
            vkBuffer = nullptr;
        }
    }

    void VKBuffer::FreeMemory()
    {
        if (vkDeviceMemory) {
            vkFreeMemory(device.GetVkDevice(), vkDeviceMemory, nullptr);
            vkDeviceMemory = nullptr;
        }
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
