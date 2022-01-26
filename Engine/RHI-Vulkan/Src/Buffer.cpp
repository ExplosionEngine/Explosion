//
// Created by johnk on 2022/1/26.
//

#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Buffer.h>

namespace RHI::Vulkan {
    VKBuffer::VKBuffer(VKDevice& d, const BufferCreateInfo* createInfo) : Buffer(createInfo), device(d)
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
        if (device.GetVkDevice().mapMemory(vkDeviceMemory, offset, length, {}, &data) != vk::Result::eSuccess) {
            throw VKException("failed to map vulkan device memory");
        }
        return data;
    }

    void VKBuffer::UnMap()
    {
        device.GetVkDevice().unmapMemory(vkDeviceMemory);
    }

    void VKBuffer::Destroy()
    {
        delete this;
    }

    void VKBuffer::CreateBuffer(const BufferCreateInfo* createInfo)
    {
        // TODO
    }

    void VKBuffer::AllocateMemory(const BufferCreateInfo* createInfo)
    {
        // TODO
    }

    void VKBuffer::DestroyBuffer()
    {
        // TODO
    }

    void VKBuffer::FreeMemory()
    {
        // TODO
    }
}
