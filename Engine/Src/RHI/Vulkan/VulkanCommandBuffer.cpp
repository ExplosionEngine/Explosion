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
            VulkanCommandEncoder commandEncoder(driver, this);
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

    void VulkanCommandBuffer::Submit(Signal* waitSignal, Signal* notifySignal, PipelineStageFlags waitStages)
    {
        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &dynamic_cast<VulkanSignal*>(waitSignal)->GetVkSemaphore();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &dynamic_cast<VulkanSignal*>(notifySignal)->GetVkSemaphore();
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

    VulkanCommandEncoder::VulkanCommandEncoder(VulkanDriver& driver, VulkanCommandBuffer* commandBuffer)
        : driver(driver), device(*driver.GetDevice()), commandBuffer(commandBuffer) {}

    VulkanCommandEncoder::~VulkanCommandEncoder() = default;

    void VulkanCommandEncoder::CopyBuffer(Buffer* srcBuffer, Buffer* dstBuffer)
    {
        VkBufferCopy bufferCopy {};
        bufferCopy.srcOffset = 0;
        bufferCopy.dstOffset = 0;
        bufferCopy.size = srcBuffer->GetSize();
        vkCmdCopyBuffer(
            commandBuffer->GetVkCommandBuffer(),
            dynamic_cast<VulkanBuffer*>(srcBuffer)->GetVkBuffer(),
            dynamic_cast<VulkanBuffer*>(dstBuffer)->GetVkBuffer(),
            1,
            &bufferCopy
        );
    }

    void VulkanCommandEncoder::BeginRenderPass(RenderPass* renderPass, const RenderPassBeginInfo& renderPassBeginInfo)
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
        beginInfo.renderPass = dynamic_cast<VulkanRenderPass*>(renderPass)->GetVkRenderPass();
        beginInfo.renderArea.offset = { renderPassBeginInfo.renderArea.x, renderPassBeginInfo.renderArea.y };
        beginInfo.renderArea.extent = { renderPassBeginInfo.renderArea.width, renderPassBeginInfo.renderArea.height };
        beginInfo.framebuffer = dynamic_cast<VulkanFrameBuffer*>(renderPassBeginInfo.frameBuffer)->GetVkFrameBuffer();
        beginInfo.clearValueCount = 1;
        beginInfo.pClearValues = &clearValue;
        vkCmdBeginRenderPass(commandBuffer->GetVkCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanCommandEncoder::EndRenderPass()
    {
        vkCmdEndRenderPass(commandBuffer->GetVkCommandBuffer());
    }

    void VulkanCommandEncoder::BindGraphicsPipeline(GraphicsPipeline* pipeline)
    {
        vkCmdBindPipeline(commandBuffer->GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, dynamic_cast<VulkanGraphicsPipeline*>(pipeline)->GetVkPipeline());
    }

    void VulkanCommandEncoder::BindVertexBuffer(uint32_t binding, Buffer* vertexBuffer)
    {
        VkDeviceSize offset[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer->GetVkCommandBuffer(), binding, 1, &dynamic_cast<VulkanBuffer*>(vertexBuffer)->GetVkBuffer(), offset);
    }

    void VulkanCommandEncoder::BindIndexBuffer(Buffer* indexBuffer)
    {
        vkCmdBindIndexBuffer(commandBuffer->GetVkCommandBuffer(), dynamic_cast<VulkanBuffer*>(indexBuffer)->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }

    void VulkanCommandEncoder::Draw(uint32_t firstVertex, uint32_t vertexCount, uint32_t firstInstance, uint32_t instanceCount)
    {
        vkCmdDraw(commandBuffer->GetVkCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandEncoder::DrawIndexed(uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset, uint32_t firstInstance, uint32_t instanceCount)
    {
        vkCmdDrawIndexed(commandBuffer->GetVkCommandBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanCommandEncoder::SetViewPort(const GraphicsPipeline::Viewport& viewport)
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

    void VulkanCommandEncoder::SetScissor(const GraphicsPipeline::Scissor& scissor)
    {
        VkRect2D sc = {
            { scissor.x, scissor.y },
            { scissor.width, scissor.height }
        };
        vkCmdSetScissor(commandBuffer->GetVkCommandBuffer(), 0, 1, &sc);
    }
}
