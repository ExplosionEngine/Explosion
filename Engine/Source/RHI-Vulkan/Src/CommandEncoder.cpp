//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandEncoder.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Gpu.h>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>
#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Texture.h>
#include <RHI/Vulkan/Common.h>
#include <RHI/Vulkan/Instance.h>
#include <RHI/Vulkan/SwapChain.h>
#include <RHI/Vulkan/BindGroup.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {

    VKCommandEncoder::VKCommandEncoder(VKDevice& dev, VKCommandBuffer& cmd)
        : device(dev), commandBuffer(cmd)
    {
    }
    VKCommandEncoder::~VKCommandEncoder()
    {
    }

    static std::tuple<VkImageLayout, VkAccessFlags, VkPipelineStageFlags> GetBarrierInfo(TextureState status)
    {
        if (status == TextureState::present) {
            return { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ACCESS_MEMORY_READ_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
        }
        if (status == TextureState::renderTarget) {
            return { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        }
        if (status == TextureState::copyDst) {
            return { VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT };
        }
        if (status == TextureState::shaderReadOnly) {
            return { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
        }
        if (status == TextureState::depthStencilReadonly) {
            return { VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT };
        }
        if (status == TextureState::depthStencilWrite) {
            return { VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT };
        }
        return {VK_IMAGE_LAYOUT_UNDEFINED, VkAccessFlags {}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
    }

    void VKCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        if (barrier.type == ResourceType::texture) {
            const auto& textureBarrierInfo = barrier.texture;
            auto oldLayout = GetBarrierInfo(textureBarrierInfo.before == TextureState::present ? TextureState::undefined : textureBarrierInfo.before);
            auto newLayout = GetBarrierInfo(textureBarrierInfo.after);

            auto* vkTexture = static_cast<VKTexture*>(textureBarrierInfo.pointer);
            VkImageMemoryBarrier imageBarrier = {};
            imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrier.image = vkTexture->GetImage();
            imageBarrier.oldLayout = std::get<0>(oldLayout);
            imageBarrier.srcAccessMask = std::get<1>(oldLayout);
            imageBarrier.newLayout = std::get<0>(newLayout);
            imageBarrier.dstAccessMask = std::get<1>(newLayout);
            imageBarrier.subresourceRange = vkTexture->GetFullRange();

            vkCmdPipelineBarrier(commandBuffer.GetVkCommandBuffer(), std::get<2>(oldLayout), std::get<2>(newLayout), VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imageBarrier);
        }
    }

    CopyPassCommandEncoder* VKCommandEncoder::BeginCopyPass()
    {
        return new VKCopyPassCommandEncoder(device, *this, commandBuffer);
    }

    ComputePassCommandEncoder* VKCommandEncoder::BeginComputePass()
    {
        return new VKComputePassCommandEncoder(device, *this, commandBuffer);
    }

    GraphicsPassCommandEncoder* VKCommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new VKGraphicsPassCommandEncoder(device, *this, commandBuffer, beginInfo);
    }

    void VKCommandEncoder::SwapChainSync(SwapChain* swapChain)
    {
        auto vkSwapChain = static_cast<VKSwapChain*>(swapChain);
        auto signal = vkSwapChain->GetImageSemaphore();
        commandBuffer.AddWaitSemaphore(signal, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        vkSwapChain->AddWaitSemaphore(commandBuffer.GetSignalSemaphores()[0]);
    }

    void VKCommandEncoder::End()
    {
        vkEndCommandBuffer(commandBuffer.GetVkCommandBuffer());
    }

    void VKCommandEncoder::Destroy()
    {
        delete this;
    }

    VKCopyPassCommandEncoder::VKCopyPassCommandEncoder(VKDevice& device, VKCommandEncoder& commandEncoder, VKCommandBuffer& commandBuffer)
        : device(device)
        , commandEncoder(commandEncoder)
        , commandBuffer(commandBuffer)
    {
    }

    VKCopyPassCommandEncoder::~VKCopyPassCommandEncoder() = default;

    void VKCopyPassCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        commandEncoder.ResourceBarrier(barrier);
    }

    void VKCopyPassCommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
        auto* srcBuffer = dynamic_cast<VKBuffer*>(src);
        auto* dstBuffer = dynamic_cast<VKBuffer*>(dst);

        VkBufferCopy copyRegion {};
        copyRegion.srcOffset = srcOffset;
        copyRegion.dstOffset = dstOffset;
        copyRegion.srcOffset = size;
        vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &copyRegion);
    }

    void VKCopyPassCommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size)
    {
        auto* buffer = dynamic_cast<VKBuffer*>(src);
        auto* texture = dynamic_cast<VKTexture*>(dst);

        VkBufferImageCopy copyRegion = {};
        copyRegion.imageExtent = { size.x, size.y, size.z };
        copyRegion.imageSubresource = { GetAspectMask(subResourceInfo->aspect), subResourceInfo->mipLevel, subResourceInfo->baseArrayLayer, subResourceInfo->arrayLayerNum };

        vkCmdCopyBufferToImage(commandBuffer.GetVkCommandBuffer(), buffer->GetVkBuffer(), texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VKCopyPassCommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Common::UVec3& size)
    {
        auto* buffer = dynamic_cast<VKBuffer*>(dst);
        auto* texture = dynamic_cast<VKTexture*>(src);

        VkBufferImageCopy copyRegion = {};
        copyRegion.imageExtent = { size.x, size.y, size.z };
        copyRegion.imageSubresource = { GetAspectMask(subResourceInfo->aspect), subResourceInfo->mipLevel, subResourceInfo->baseArrayLayer, subResourceInfo->arrayLayerNum };

        vkCmdCopyImageToBuffer(commandBuffer.GetVkCommandBuffer(), texture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->GetVkBuffer(), 1, &copyRegion);
    }

    void VKCopyPassCommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo,
        Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Common::UVec3& size)
    {
        auto* srcTexture = dynamic_cast<VKTexture*>(src);
        auto* dstTexture = dynamic_cast<VKTexture*>(dst);

        VkImageCopy copyRegion = {};
        copyRegion.extent = {size.x, size.y, size.z };
        copyRegion.srcSubresource = { GetAspectMask(srcSubResourceInfo->aspect), srcSubResourceInfo->mipLevel, srcSubResourceInfo->baseArrayLayer, srcSubResourceInfo->arrayLayerNum };
        copyRegion.dstSubresource = { GetAspectMask(dstSubResourceInfo->aspect), dstSubResourceInfo->mipLevel, dstSubResourceInfo->baseArrayLayer, dstSubResourceInfo->arrayLayerNum };

        vkCmdCopyImage(commandBuffer.GetVkCommandBuffer(), srcTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VKCopyPassCommandEncoder::EndPass()
    {
    }

    void VKCopyPassCommandEncoder::Destroy()
    {
        delete this;
    }

    VKComputePassCommandEncoder::VKComputePassCommandEncoder(VKDevice& device, VKCommandEncoder& commandEncoder, VKCommandBuffer& commandBuffer)
        : device(device)
        , commandEncoder(commandEncoder)
        , commandBuffer(commandBuffer)
    {
    }

    VKComputePassCommandEncoder::~VKComputePassCommandEncoder() = default;

    void VKComputePassCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        commandEncoder.ResourceBarrier(barrier);
    }

    void VKComputePassCommandEncoder::SetPipeline(ComputePipeline* pipeline)
    {
        // TODO
    }

    void VKComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
        // TODO
    }

    void VKComputePassCommandEncoder::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
    {
        // TODO
    }

    void VKComputePassCommandEncoder::EndPass()
    {
        // TODO
    }

    void VKComputePassCommandEncoder::Destroy()
    {
        delete this;
    }

    VKGraphicsPassCommandEncoder::VKGraphicsPassCommandEncoder(VKDevice& dev, VKCommandEncoder& commandEncoder, VKCommandBuffer& cmd,const GraphicsPassBeginInfo* beginInfo)
        : device(dev)
        , commandEncoder(commandEncoder)
        , commandBuffer(cmd)
    {
        std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(beginInfo->colorAttachmentNum);
        for (size_t i = 0; i < beginInfo->colorAttachmentNum; i++)
        {
            auto* colorTextureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[i].view);
            colorAttachmentInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorAttachmentInfos[i].imageView = colorTextureView->GetVkImageView();
            colorAttachmentInfos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfos[i].loadOp = VKEnumCast<LoadOp, VkAttachmentLoadOp>(beginInfo->colorAttachments[i].loadOp);
            colorAttachmentInfos[i].storeOp = VKEnumCast<StoreOp, VkAttachmentStoreOp>(beginInfo->colorAttachments[i].storeOp);
            colorAttachmentInfos[i].clearValue.color = {
                    beginInfo->colorAttachments[i].clearValue.r,
                    beginInfo->colorAttachments[i].clearValue.g,
                    beginInfo->colorAttachments[i].clearValue.b,
                    beginInfo->colorAttachments[i].clearValue.a
                    };
        }

        auto* textureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[0].view);
        VkRenderingInfoKHR renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.colorAttachmentCount = colorAttachmentInfos.size();
        renderingInfo.pColorAttachments = colorAttachmentInfos.data();
        renderingInfo.layerCount = textureView->GetArrayLayerNum();
        renderingInfo.renderArea = {{0, 0}, {static_cast<uint32_t>(textureView->GetTexture().GetExtent().x), static_cast<uint32_t>(textureView->GetTexture().GetExtent().y)}};
        renderingInfo.viewMask = 0;

        if (beginInfo->depthStencilAttachment != nullptr)
        {
            auto* depthStencilTextureView = dynamic_cast<VKTextureView*>(beginInfo->depthStencilAttachment->view);

            VkRenderingAttachmentInfo depthAttachmentInfo = {};
            depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            depthAttachmentInfo.imageView = depthStencilTextureView->GetVkImageView();
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachmentInfo.loadOp = VKEnumCast<LoadOp, VkAttachmentLoadOp>(beginInfo->depthStencilAttachment->depthLoadOp);
            depthAttachmentInfo.storeOp = VKEnumCast<StoreOp, VkAttachmentStoreOp>(beginInfo->depthStencilAttachment->depthStoreOp);
            depthAttachmentInfo.clearValue.depthStencil = { beginInfo->depthStencilAttachment->depthClearValue, beginInfo->depthStencilAttachment->stencilClearValue };

            renderingInfo.pDepthAttachment = &depthAttachmentInfo;

            if (!beginInfo->depthStencilAttachment->depthReadOnly) {
                VkRenderingAttachmentInfo stencilAttachmentInfo = {};
                stencilAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                stencilAttachmentInfo.imageView = depthStencilTextureView->GetVkImageView();
                stencilAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                stencilAttachmentInfo.loadOp = VKEnumCast<LoadOp, VkAttachmentLoadOp>(beginInfo->depthStencilAttachment->stencilLoadOp);
                stencilAttachmentInfo.storeOp = VKEnumCast<StoreOp, VkAttachmentStoreOp>(beginInfo->depthStencilAttachment->stencilStoreOp);
                stencilAttachmentInfo.clearValue.depthStencil = { beginInfo->depthStencilAttachment->depthClearValue, beginInfo->depthStencilAttachment->stencilClearValue };

                renderingInfo.pStencilAttachment = &stencilAttachmentInfo;
            }
        }

        cmdHandle = cmd.GetVkCommandBuffer();
        device.GetGpu().GetInstance().vkCmdBeginRenderingKHR(cmdHandle, &renderingInfo);
    }

    VKGraphicsPassCommandEncoder::~VKGraphicsPassCommandEncoder() = default;

    void VKGraphicsPassCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        commandEncoder.ResourceBarrier(barrier);
    }

    void VKGraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* pipeline)
    {
        graphicsPipeline = dynamic_cast<VKGraphicsPipeline*>(pipeline);
        Assert(graphicsPipeline);

       vkCmdBindPipeline(cmdHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetVkPipeline());
    }

    void VKGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
        auto* vBindGroup = dynamic_cast<VKBindGroup*>(bindGroup);
        VkDescriptorSet descriptorSet = vBindGroup->GetVkDescritorSet();
        VkPipelineLayout layout = graphicsPipeline->GetPipelineLayout()->GetVkPipelineLayout();

        vkCmdBindDescriptorSets(cmdHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, layoutIndex, 1, &descriptorSet, 0, nullptr);
    }

    void VKGraphicsPassCommandEncoder::SetIndexBuffer(BufferView *bufferView)
    {
        auto* mBufferView = dynamic_cast<VKBufferView*>(bufferView);

        VkBuffer indexBuffer = mBufferView->GetBuffer().GetVkBuffer();
        auto vkFormat = VKEnumCast<IndexFormat, VkIndexType>(mBufferView->GetIndexFormat());

        vkCmdBindIndexBuffer(cmdHandle, indexBuffer, 0, vkFormat);
    }

    void VKGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView *bufferView)
    {
        auto* mBufferView = dynamic_cast<VKBufferView*>(bufferView);

        VkBuffer vertexBuffer = mBufferView->GetBuffer().GetVkBuffer();
        VkDeviceSize offset[] = { mBufferView->GetOffset() };
        vkCmdBindVertexBuffers(cmdHandle,slot, 1, &vertexBuffer, offset);
    }

    void VKGraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
        vkCmdDraw(cmdHandle, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VKGraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
        vkCmdDrawIndexed(cmdHandle, indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
    }

    void VKGraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        vkCmdSetViewport(cmdHandle, 0, 1, &viewport);
    }

    void VKGraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        VkRect2D rect;
        rect.offset = { static_cast<int32_t>(left), static_cast<int32_t>(top) };
        rect.extent = { right - left, bottom - top };
        vkCmdSetScissor(cmdHandle, 0, 1, &rect);
    }

    void VKGraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
    {
        // check extension
//        cmdHandle.setPrimitiveTopologyEXT(VKEnumCast<PrimitiveTopologyType, vk::PrimitiveTopology>(primitiveTopology)
    }

    void VKGraphicsPassCommandEncoder::SetBlendConstant(const float *constants)
    {
        vkCmdSetBlendConstants(cmdHandle, constants);
    }

    void VKGraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
        // TODO stencil face;
        vkCmdSetStencilReference(cmdHandle, VK_STENCIL_FACE_FRONT_AND_BACK, reference);
    }

    void VKGraphicsPassCommandEncoder::EndPass()
    {
        device.GetGpu().GetInstance().vkCmdEndRenderingKHR(cmdHandle);
    }

    void VKGraphicsPassCommandEncoder::Destroy()
    {
        delete this;
    }
}
