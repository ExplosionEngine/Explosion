//
// Created by John Kindem on 2021/3/30.
//

#include <stdexcept>

#include <Explosion/RHI/Vulkan/VulkanCommandBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanSignal.h>
#include <Explosion/RHI/Vulkan/VulkanRenderPass.h>
#include <Explosion/RHI/Vulkan/VulkanFrameBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanGraphicsPipeline.h>
#include <Explosion/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanDriver& driver)
        : driver(driver), device(*driver.GetDevice())
    {
        AllocateCommandBuffer();
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        FreeCommandBuffer();
    }

    const VkCommandBuffer& VulkanCommandBuffer::GetVkCommandBuffer()
    {
        return vkCommandBuffer;
    }

    void VulkanCommandBuffer::EncodeCommands(const EncodingFunc& encodingFunc)
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

    void VulkanCommandBuffer::SubmitNow()
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

    void VulkanCommandBuffer::Submit(VulkanSignal* waitSignal, VulkanSignal* notifySignal, PipelineStageFlags waitStages)
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
        VkPipelineStageFlags flags = VkGetFlags<PipelineStageBits, VkPipelineStageFlagBits>(waitStages);
        submitInfo.pWaitDstStageMask = &flags;

        if (vkQueueSubmit(device.GetVkQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit vulkan command buffer");
        }
    }

    void VulkanCommandBuffer::AllocateCommandBuffer()
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

    void VulkanCommandBuffer::FreeCommandBuffer()
    {
        vkFreeCommandBuffers(device.GetVkDevice(), device.GetVkCommandPool(), 1, &vkCommandBuffer);
    }

    CommandEncoder::CommandEncoder(VulkanDriver& driver, VulkanCommandBuffer* commandBuffer)
        : driver(driver), device(*driver.GetDevice()), commandBuffer(commandBuffer) {}

    CommandEncoder::~CommandEncoder() = default;

    void CommandEncoder::CopyBuffer(VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer)
    {
        VkBufferCopy bufferCopy {};
        bufferCopy.srcOffset = 0;
        bufferCopy.dstOffset = 0;
        bufferCopy.size = srcBuffer->GetSize();
        vkCmdCopyBuffer(commandBuffer->GetVkCommandBuffer(), srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &bufferCopy);
    }

    void CommandEncoder::BeginRenderPass(VulkanRenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo)
    {
        VkClearValue clearValue = {
            renderPassBeginInfo.clearValue.r,
            renderPassBeginInfo.clearValue.g,
            renderPassBeginInfo.clearValue.b,
            renderPassBeginInfo.clearValue.a
        };

        VkRenderPassBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.pNext = nullptr;
        beginInfo.renderPass = renderPass->GetVkRenderPass();
        beginInfo.renderArea.offset = { renderPassBeginInfo.renderArea.x, renderPassBeginInfo.renderArea.y };
        beginInfo.renderArea.extent = { renderPassBeginInfo.renderArea.width, renderPassBeginInfo.renderArea.height };
        beginInfo.framebuffer = renderPassBeginInfo.frameBuffer->GetVkFrameBuffer();
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &clearValue;
        vkCmdBeginRenderPass(commandBuffer->GetVkCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandEncoder::EndRenderPass()
    {
        vkCmdEndRenderPass(commandBuffer->GetVkCommandBuffer());
    }

    void CommandEncoder::BindGraphicsPipeline(VulkanGraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(commandBuffer->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetVkPipeline());
    }

    void CommandEncoder::BindVertexBuffer(uint32_t binding, VulkanBuffer* vertexBuffer)
    {
        VkDeviceSize offset[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer->GetVkCommandBuffer(), binding, 1, &vertexBuffer->GetVkBuffer(), offset);
    }

    void CommandEncoder::BindIndexBuffer(VulkanBuffer* indexBuffer)
    {
        vkCmdBindIndexBuffer(commandBuffer->GetVkCommandBuffer(), indexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void CommandEncoder::Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount)
    {
        vkCmdDraw(commandBuffer->GetVkCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandEncoder::DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount)
    {
        vkCmdDrawIndexed(commandBuffer->GetVkCommandBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandEncoder::SetViewPort(const VulkanGraphicsPipeline::Viewport& viewport)
    {
        VkViewport vp = {
            viewport.x,
            viewport.y,
            viewport.width,
            viewport.height,
            viewport.minDepth,
            viewport.maxDepth
        };
        vkCmdSetViewport(commandBuffer->GetVkCommandBuffer(), 0, 1, &vp);
    }

    void CommandEncoder::SetScissor(const VulkanGraphicsPipeline::Scissor& scissor)
    {
        VkRect2D sc = {
            { scissor.x, scissor.y },
            { scissor.width, scissor.height }
        };
        vkCmdSetScissor(commandBuffer->GetVkCommandBuffer(), 0, 1, &sc);
    }
}
