//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/CommandBuffer.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {
    CommandBuffer::CommandBuffer(Driver& driver)
        : driver(driver), device(*driver.GetDevice())
    {
        AllocateCommandBuffer();
    }

    CommandBuffer::~CommandBuffer()
    {
        FreeCommandBuffer();
    }

    const VkCommandBuffer& CommandBuffer::GetVkCommandBuffer()
    {
        return vkCommandBuffer;
    }

    void CommandBuffer::AllocateCommandBuffer()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.pNext = nullptr;
        commandBufferAllocateInfo.commandPool = device.GetVkCommandPool();
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(device.GetVkDevice(), &commandBufferAllocateInfo, &vkCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vulkan command buffer");
        }
    }

    void CommandBuffer::FreeCommandBuffer()
    {
        vkFreeCommandBuffers(device.GetVkDevice(), device.GetVkCommandPool(), 1, &vkCommandBuffer);
    }
}
