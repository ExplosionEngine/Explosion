//
// Created by John Kindem on 2021/3/30.
//

#include <Explosion/Driver/CommandBuffer.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/CommandEncoder.h>
#include <Explosion/Driver/Signal.h>

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

    void CommandBuffer::EncodeCommands(const EncodingFunc& encodingFunc)
    {
        VkCommandBufferBeginInfo commandBufferBeginInfo {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.pNext = nullptr;
        commandBufferBeginInfo.flags = 0;
        commandBufferBeginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(vkCommandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin vulkan command buffer");
        }

        {
            CommandEncoder commandEncoder(driver, this);
            encodingFunc(&commandEncoder);
        }

        if (vkEndCommandBuffer(vkCommandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to end vulkan command buffer");
        }
    }

    void CommandBuffer::SubmitNow()
    {
        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        if (vkQueueSubmit(device.GetVkQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit vulkan command buffer");
        }
        vkQueueWaitIdle(device.GetVkQueue());
    }

    void CommandBuffer::Submit(Signal* waitSignal, Signal* notifySignal)
    {
        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSignal->GetVkSemaphore();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &notifySignal->GetVkSemaphore();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        if (vkQueueSubmit(device.GetVkQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit vulkan command buffer");
        }
    }

    void CommandBuffer::SetupSubmitInfo(VkSubmitInfo& submitInfo) {}

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

    FrameOutputCommandBuffer::FrameOutputCommandBuffer(Driver& driver) : CommandBuffer(driver) {}

    FrameOutputCommandBuffer::~FrameOutputCommandBuffer() = default;

    void FrameOutputCommandBuffer::SetupSubmitInfo(VkSubmitInfo& submitInfo)
    {
        CommandBuffer::SetupSubmitInfo(submitInfo);

        VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = &waitStages;
    }
}
