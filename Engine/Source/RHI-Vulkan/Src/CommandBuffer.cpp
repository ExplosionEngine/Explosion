//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/CommandEncoder.h>
#include <Common/Debug.h>

namespace RHI::Vulkan {

    VKCommandBuffer::VKCommandBuffer(VKDevice& dev, VkCommandPool p)
        : device(dev)
        , pool(p)
    {
        CreateNativeCommandBuffer();
    }

    VKCommandBuffer::~VKCommandBuffer()
    {
        auto vkDevice = device.GetVkDevice();
        if (commandBuffer) {
            vkFreeCommandBuffers(vkDevice, pool, 1, &commandBuffer);
        }
    }

    void VKCommandBuffer::Destroy()
    {
        delete this;
    }

    CommandEncoder* VKCommandBuffer::Begin()
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        // TODO maybe expose this to create info ?
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return new VKCommandEncoder(device, *this);
    }

    VkCommandBuffer VKCommandBuffer::GetVkCommandBuffer() const
    {
        return commandBuffer;
    }

    void VKCommandBuffer::CreateNativeCommandBuffer()
    {
        VkCommandBufferAllocateInfo cmdInfo = {};
        cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdInfo.commandBufferCount = 1;
        cmdInfo.commandPool = pool;
        cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        Assert(vkAllocateCommandBuffers(device.GetVkDevice(), &cmdInfo, &commandBuffer) == VK_SUCCESS);
    }
}