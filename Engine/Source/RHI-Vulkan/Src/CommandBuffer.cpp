//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/CommandEncoder.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {

    VKCommandBuffer::VKCommandBuffer(VKDevice& dev, vk::CommandPool p) : device(dev), pool(p)
    {
        CreateNativeCommandBuffer();
    }

    VKCommandBuffer::~VKCommandBuffer()
    {
        if (commandBuffer) {
            device.GetVkDevice().freeCommandBuffers(pool, 1, &commandBuffer);
        }
    }

    void VKCommandBuffer::Destroy()
    {
        delete this;
    }

    CommandEncoder* VKCommandBuffer::Begin()
    {
        vk::CommandBufferBeginInfo beginInfo = {};
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commandBuffer.begin(&beginInfo);
        return new VKCommandEncoder(device, *this);
    }

    vk::CommandBuffer VKCommandBuffer::GetNativeHandle() const
    {
        return commandBuffer;
    }

    void VKCommandBuffer::CreateNativeCommandBuffer()
    {
        vk::CommandBufferAllocateInfo cmdInfo;
        cmdInfo.setCommandBufferCount(1)
            .setCommandPool(pool)
            .setLevel(vk::CommandBufferLevel::ePrimary);

        Assert(device.GetVkDevice().allocateCommandBuffers(&cmdInfo, &commandBuffer) == vk::Result::eSuccess);
    }

}