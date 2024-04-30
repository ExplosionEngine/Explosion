//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandRecorder.h>
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
#include <RHI/Vulkan/BindGroup.h>
#include <RHI/Vulkan/PipelineLayout.h>
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {
    static VkAccessFlags GetBufferMemoryBarrierAccessFlags(BufferState inState)
    {
        static std::unordered_map<BufferState, VkAccessFlags> map = {
            { BufferState::undefined, VK_ACCESS_NONE },
            { BufferState::staging, VK_ACCESS_HOST_WRITE_BIT },
            { BufferState::copySrc, VK_ACCESS_TRANSFER_READ_BIT },
            { BufferState::copyDst, VK_ACCESS_TRANSFER_WRITE_BIT },
            { BufferState::shaderReadOnly, VK_ACCESS_SHADER_READ_BIT },
            { BufferState::storage, VK_ACCESS_SHADER_WRITE_BIT }
        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetBufferPipelineBarrierSrcStage(BufferState inState)
    {
        static std::unordered_map<BufferState, VkPipelineStageFlags> map = {
            { BufferState::undefined, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },
            { BufferState::staging, VK_PIPELINE_STAGE_HOST_BIT },
            { BufferState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }
        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetBufferPipelineBarrierDstStage(BufferState inState)
    {
        static std::unordered_map<BufferState, VkPipelineStageFlags> map = {
            { BufferState::undefined, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT },
            { BufferState::staging, VK_PIPELINE_STAGE_HOST_BIT },
            { BufferState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }
        };
        return map.at(inState);
    }

    static VkAccessFlags GetTextureMemoryBarrierAccessFlags(TextureState inState)
    {
        static std::unordered_map<TextureState, VkAccessFlags> map = {
            { TextureState::undefined, VK_ACCESS_NONE },
            { TextureState::copySrc, VK_ACCESS_TRANSFER_READ_BIT },
            { TextureState::copyDst, VK_ACCESS_TRANSFER_WRITE_BIT },
            { TextureState::shaderReadOnly, VK_ACCESS_SHADER_READ_BIT },
            { TextureState::renderTarget, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT },
            { TextureState::storage, VK_ACCESS_SHADER_WRITE_BIT },
            { TextureState::depthStencilReadonly, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT },
            { TextureState::depthStencilWrite, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT },
            { TextureState::present, VK_ACCESS_MEMORY_READ_BIT }
        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetTexturePipelineBarrierSrcStage(TextureState inState)
    {
        static std::unordered_map<TextureState, VkPipelineStageFlags> map = {
            { TextureState::undefined, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },
            { TextureState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { TextureState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { TextureState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { TextureState::renderTarget, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
            { TextureState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { TextureState::depthStencilReadonly, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT },
            { TextureState::depthStencilWrite, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT },
            { TextureState::present, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT }
        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetTexturePipelineBarrierDstStage(TextureState inState)
    {
        static std::unordered_map<TextureState, VkPipelineStageFlags> map = {
            { TextureState::undefined, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT },
            { TextureState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { TextureState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { TextureState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { TextureState::renderTarget, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
            { TextureState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { TextureState::depthStencilReadonly, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT },
            { TextureState::depthStencilWrite, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT },
            { TextureState::present, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT }
        };
        return map.at(inState);
    }

    static VkImageLayout GetTextureLayout(TextureState inState)
    {
        static std::unordered_map<TextureState, VkImageLayout> map = {
            { TextureState::undefined, VK_IMAGE_LAYOUT_UNDEFINED },
            { TextureState::copySrc, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL },
            { TextureState::copyDst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
            { TextureState::shaderReadOnly, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
            { TextureState::renderTarget, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
            { TextureState::storage, VK_IMAGE_LAYOUT_GENERAL },
            { TextureState::depthStencilReadonly, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL },
            { TextureState::depthStencilWrite, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
            { TextureState::present, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }
        };
        return map.at(inState);
    }
}

namespace RHI::Vulkan {
    VulkanCommandRecorder::VulkanCommandRecorder(VulkanDevice& inDevice, VulkanCommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandBuffer(inCmdBuffer)
    {
    }

    VulkanCommandRecorder::~VulkanCommandRecorder() = default;

    void VulkanCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        if (inBarrier.type == ResourceType::buffer) {
            const auto& bufferBarrierInfo = inBarrier.buffer;
            auto* nativeBuffer = static_cast<VulkanBuffer*>(bufferBarrierInfo.pointer);

            VkBufferMemoryBarrier bufferBarrier {};
            bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            bufferBarrier.buffer = nativeBuffer->GetNative();
            bufferBarrier.size = nativeBuffer->GetCreateInfo().size;
            bufferBarrier.offset = 0;
            bufferBarrier.srcAccessMask = GetBufferMemoryBarrierAccessFlags(bufferBarrierInfo.before);
            bufferBarrier.dstAccessMask = GetBufferMemoryBarrierAccessFlags(bufferBarrierInfo.after);

            vkCmdPipelineBarrier(
                commandBuffer.GetNativeCommandBuffer(),
                GetBufferPipelineBarrierSrcStage(bufferBarrierInfo.before), GetBufferPipelineBarrierDstStage(bufferBarrierInfo.after),
                VK_DEPENDENCY_BY_REGION_BIT,
                0, nullptr,
                1, &bufferBarrier,
                0, nullptr);
        } else if (inBarrier.type == ResourceType::texture) {
            const auto& textureBarrierInfo = inBarrier.texture;

            auto* nativeTexture = static_cast<VulkanTexture*>(textureBarrierInfo.pointer);
            VkImageMemoryBarrier imageBarrier {};
            imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrier.image = nativeTexture->GetNative();
            imageBarrier.oldLayout = GetTextureLayout(textureBarrierInfo.before);
            imageBarrier.srcAccessMask = GetTextureMemoryBarrierAccessFlags(textureBarrierInfo.before);
            imageBarrier.newLayout = GetTextureLayout(textureBarrierInfo.after);
            imageBarrier.dstAccessMask = GetTextureMemoryBarrierAccessFlags(textureBarrierInfo.after);
            imageBarrier.subresourceRange = nativeTexture->GetNativeSubResourceFullRange();

            vkCmdPipelineBarrier(
                commandBuffer.GetNativeCommandBuffer(),
                GetTexturePipelineBarrierSrcStage(textureBarrierInfo.before), GetTexturePipelineBarrierDstStage(textureBarrierInfo.after),
                VK_DEPENDENCY_BY_REGION_BIT,
                0, nullptr,
                0, nullptr,
                1, &imageBarrier);
        } else {
            Unimplement();
        }
    }

    Common::UniqueRef<CopyPassCommandRecorder> VulkanCommandRecorder::BeginCopyPass()
    {
        return Common::UniqueRef<CopyPassCommandRecorder>(new VulkanCopyPassCommandRecorder(device, *this, commandBuffer));
    }

    Common::UniqueRef<ComputePassCommandRecorder> VulkanCommandRecorder::BeginComputePass()
    {
        return Common::UniqueRef<ComputePassCommandRecorder>(new VulkanComputePassCommandRecorder(device, *this, commandBuffer));
    }

    Common::UniqueRef<RasterPassCommandRecorder> VulkanCommandRecorder::BeginRasterPass(const RasterPassBeginInfo& inBeginInfo)
    {
        return Common::UniqueRef<RasterPassCommandRecorder>(new VulkanRasterPassCommandRecorder(device, *this, commandBuffer, inBeginInfo));
    }

    void VulkanCommandRecorder::End()
    {
        vkEndCommandBuffer(commandBuffer.GetNativeCommandBuffer());
    }

    VulkanCopyPassCommandRecorder::VulkanCopyPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
    {
    }

    VulkanCopyPassCommandRecorder::~VulkanCopyPassCommandRecorder() = default;

    void VulkanCopyPassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void VulkanCopyPassCommandRecorder::CopyBufferToBuffer(Buffer* inSrcBuffer, size_t inSrcOffset, Buffer* inDestBuffer, size_t inDestOffset, size_t inSize)
    {
        auto* srcBuffer = static_cast<VulkanBuffer*>(inSrcBuffer);
        auto* dstBuffer = static_cast<VulkanBuffer*>(inDestBuffer);

        VkBufferCopy copyRegion {};
        copyRegion.srcOffset = inSrcOffset;
        copyRegion.dstOffset = inDestOffset;
        copyRegion.srcOffset = inSize;
        vkCmdCopyBuffer(commandBuffer.GetNativeCommandBuffer(), srcBuffer->GetNative(), dstBuffer->GetNative(), 1, &copyRegion);
    }

    void VulkanCopyPassCommandRecorder::CopyBufferToTexture(Buffer* inSrcBuffer, Texture* inDestTexture, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize)
    {
        auto* buffer = static_cast<VulkanBuffer*>(inSrcBuffer);
        auto* texture = static_cast<VulkanTexture*>(inDestTexture);

        VkBufferImageCopy copyRegion = {};
        copyRegion.imageExtent = {inSize.x, inSize.y, inSize.z };
        copyRegion.imageSubresource = { EnumCast<TextureAspect, VkImageAspectFlags>(inSubResourceInfo->aspect), inSubResourceInfo->mipLevel, inSubResourceInfo->baseArrayLayer, inSubResourceInfo->arrayLayerNum };

        vkCmdCopyBufferToImage(commandBuffer.GetNativeCommandBuffer(), buffer->GetNative(), texture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VulkanCopyPassCommandRecorder::CopyTextureToBuffer(Texture* inSrcTexture, Buffer* inDestBuffer, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize)
    {
        auto* buffer = static_cast<VulkanBuffer*>(inDestBuffer);
        auto* texture = static_cast<VulkanTexture*>(inSrcTexture);

        VkBufferImageCopy copyRegion = {};
        copyRegion.imageExtent = {inSize.x, inSize.y, inSize.z };
        copyRegion.imageSubresource = { EnumCast<TextureAspect, VkImageAspectFlags>(inSubResourceInfo->aspect), inSubResourceInfo->mipLevel, inSubResourceInfo->baseArrayLayer, inSubResourceInfo->arrayLayerNum };

        vkCmdCopyImageToBuffer(commandBuffer.GetNativeCommandBuffer(), texture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               buffer->GetNative(), 1, &copyRegion);
    }

    void VulkanCopyPassCommandRecorder::CopyTextureToTexture(Texture* inSrcTexture, const TextureSubResourceInfo* inSrcSubResourceInfo, Texture* inDestTexture, const TextureSubResourceInfo* inDestSubResourceInfo, const Common::UVec3& inSize)
    {
        auto* srcTexture = static_cast<VulkanTexture*>(inSrcTexture);
        auto* dstTexture = static_cast<VulkanTexture*>(inDestTexture);

        VkImageCopy copyRegion = {};
        copyRegion.extent = {inSize.x, inSize.y, inSize.z };
        copyRegion.srcSubresource = { EnumCast<TextureAspect, VkImageAspectFlags>(inSrcSubResourceInfo->aspect), inSrcSubResourceInfo->mipLevel, inSrcSubResourceInfo->baseArrayLayer, inSrcSubResourceInfo->arrayLayerNum };
        copyRegion.dstSubresource = { EnumCast<TextureAspect, VkImageAspectFlags>(inDestSubResourceInfo->aspect), inDestSubResourceInfo->mipLevel, inDestSubResourceInfo->baseArrayLayer, inDestSubResourceInfo->arrayLayerNum };

        vkCmdCopyImage(commandBuffer.GetNativeCommandBuffer(), srcTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
    }

    void VulkanCopyPassCommandRecorder::EndPass()
    {
    }

    VulkanComputePassCommandRecorder::VulkanComputePassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
    {
    }

    VulkanComputePassCommandRecorder::~VulkanComputePassCommandRecorder() = default;

    void VulkanComputePassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void VulkanComputePassCommandRecorder::SetPipeline(ComputePipeline* inPipeline)
    {
        // TODO
    }

    void VulkanComputePassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        // TODO
    }

    void VulkanComputePassCommandRecorder::Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ)
    {
        // TODO
    }

    void VulkanComputePassCommandRecorder::EndPass()
    {
        // TODO
    }

    VulkanRasterPassCommandRecorder::VulkanRasterPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer, const RasterPassBeginInfo& inBeginInfo)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
    {
        std::vector<VkRenderingAttachmentInfo> colorAttachmentInfos(inBeginInfo.colorAttachments.size());
        for (size_t i = 0; i < inBeginInfo.colorAttachments.size(); i++)
        {
            auto* colorTextureView = static_cast<VulkanTextureView*>(inBeginInfo.colorAttachments[i].view);
            colorAttachmentInfos[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorAttachmentInfos[i].imageView = colorTextureView->GetNative();
            colorAttachmentInfos[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfos[i].loadOp = EnumCast<LoadOp, VkAttachmentLoadOp>(inBeginInfo.colorAttachments[i].loadOp);
            colorAttachmentInfos[i].storeOp = EnumCast<StoreOp, VkAttachmentStoreOp>(inBeginInfo.colorAttachments[i].storeOp);
            colorAttachmentInfos[i].clearValue.color = {
                inBeginInfo.colorAttachments[i].clearValue.r,
                inBeginInfo.colorAttachments[i].clearValue.g,
                inBeginInfo.colorAttachments[i].clearValue.b,
                inBeginInfo.colorAttachments[i].clearValue.a
                    };
        }

        auto* textureView = static_cast<VulkanTextureView*>(inBeginInfo.colorAttachments[0].view);
        VkRenderingInfoKHR renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.colorAttachmentCount = colorAttachmentInfos.size();
        renderingInfo.pColorAttachments = colorAttachmentInfos.data();
        renderingInfo.layerCount = textureView->GetArrayLayerNum();
        renderingInfo.renderArea = {{0, 0}, {static_cast<uint32_t>(textureView->GetTexture().GetExtent().x), static_cast<uint32_t>(textureView->GetTexture().GetExtent().y)}};
        renderingInfo.viewMask = 0;

        if (inBeginInfo.depthStencilAttachment.has_value())
        {
            auto* depthStencilTextureView = static_cast<VulkanTextureView*>(inBeginInfo.depthStencilAttachment->view);

            VkRenderingAttachmentInfo depthAttachmentInfo = {};
            depthAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            depthAttachmentInfo.imageView = depthStencilTextureView->GetNative();
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachmentInfo.loadOp = EnumCast<LoadOp, VkAttachmentLoadOp>(inBeginInfo.depthStencilAttachment->depthLoadOp);
            depthAttachmentInfo.storeOp = EnumCast<StoreOp, VkAttachmentStoreOp>(inBeginInfo.depthStencilAttachment->depthStoreOp);
            depthAttachmentInfo.clearValue.depthStencil = {inBeginInfo.depthStencilAttachment->depthClearValue, inBeginInfo.depthStencilAttachment->stencilClearValue };

            renderingInfo.pDepthAttachment = &depthAttachmentInfo;

            if (!inBeginInfo.depthStencilAttachment->depthReadOnly) {
                VkRenderingAttachmentInfo stencilAttachmentInfo = {};
                stencilAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                stencilAttachmentInfo.imageView = depthStencilTextureView->GetNative();
                stencilAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                stencilAttachmentInfo.loadOp = EnumCast<LoadOp, VkAttachmentLoadOp>(inBeginInfo.depthStencilAttachment->stencilLoadOp);
                stencilAttachmentInfo.storeOp = EnumCast<StoreOp, VkAttachmentStoreOp>(inBeginInfo.depthStencilAttachment->stencilStoreOp);
                stencilAttachmentInfo.clearValue.depthStencil = {inBeginInfo.depthStencilAttachment->depthClearValue, inBeginInfo.depthStencilAttachment->stencilClearValue };

                renderingInfo.pStencilAttachment = &stencilAttachmentInfo;
            }
        }

        nativeCmdBuffer = inCmdBuffer.GetNativeCommandBuffer();

        auto* pfn = device.GetGpu().GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkCmdBeginRenderingKHR>("vkCmdBeginRenderingKHR");
        pfn(nativeCmdBuffer, &renderingInfo);
    }

    VulkanRasterPassCommandRecorder::~VulkanRasterPassCommandRecorder() = default;

    void VulkanRasterPassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void VulkanRasterPassCommandRecorder::SetPipeline(RasterPipeline* inPipeline)
    {
        rasterPipeline = static_cast<VulkanRasterPipeline*>(inPipeline);
        Assert(rasterPipeline);

       vkCmdBindPipeline(nativeCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rasterPipeline->GetNative());
    }

    void VulkanRasterPassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        auto* vBindGroup = static_cast<VulkanBindGroup*>(inBindGroup);
        VkDescriptorSet descriptorSet = vBindGroup->GetNative();
        VkPipelineLayout layout = rasterPipeline->GetPipelineLayout()->GetNative();

        vkCmdBindDescriptorSets(nativeCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, inLayoutIndex, 1, &descriptorSet, 0, nullptr);
    }

    void VulkanRasterPassCommandRecorder::SetIndexBuffer(BufferView *inBufferView)
    {
        auto* mBufferView = static_cast<VulkanBufferView*>(inBufferView);

        VkBuffer indexBuffer = mBufferView->GetBuffer().GetNative();
        auto vkFormat = EnumCast<IndexFormat, VkIndexType>(mBufferView->GetIndexFormat());

        vkCmdBindIndexBuffer(nativeCmdBuffer, indexBuffer, 0, vkFormat);
    }

    void VulkanRasterPassCommandRecorder::SetVertexBuffer(size_t inSlot, BufferView *inBufferView)
    {
        auto* mBufferView = static_cast<VulkanBufferView*>(inBufferView);

        VkBuffer vertexBuffer = mBufferView->GetBuffer().GetNative();
        VkDeviceSize offset[] = { mBufferView->GetOffset() };
        vkCmdBindVertexBuffers(nativeCmdBuffer, inSlot, 1, &vertexBuffer, offset);
    }

    void VulkanRasterPassCommandRecorder::Draw(size_t inVertexCount, size_t inInstanceCount, size_t inFirstVertex, size_t inFirstInstance)
    {
        vkCmdDraw(nativeCmdBuffer, inVertexCount, inInstanceCount, inFirstVertex, inFirstInstance);
    }

    void VulkanRasterPassCommandRecorder::DrawIndexed(size_t inIndexCount, size_t inInstanceCount, size_t inFirstIndex, size_t inBaseVertex, size_t inFirstInstance)
    {
        vkCmdDrawIndexed(nativeCmdBuffer, inIndexCount, inInstanceCount, inFirstIndex, inBaseVertex, inFirstInstance);
    }

    void VulkanRasterPassCommandRecorder::SetViewport(float inX, float inY, float inWidth, float inHeight, float inMinDepth, float inMaxDepth)
    {
        VkViewport viewport{};
        viewport.x = inX;
        viewport.y = inY;
        viewport.width = inWidth;
        viewport.height = inHeight;
        viewport.minDepth = inMinDepth;
        viewport.maxDepth = inMaxDepth;
        vkCmdSetViewport(nativeCmdBuffer, 0, 1, &viewport);
    }

    void VulkanRasterPassCommandRecorder::SetScissor(uint32_t inLeft, uint32_t inTop, uint32_t inRight, uint32_t inBottom)
    {
        VkRect2D rect;
        rect.offset = {static_cast<int32_t>(inLeft), static_cast<int32_t>(inTop) };
        rect.extent = {inRight - inLeft, inBottom - inTop };
        vkCmdSetScissor(nativeCmdBuffer, 0, 1, &rect);
    }

    void VulkanRasterPassCommandRecorder::SetPrimitiveTopology(PrimitiveTopology inPrimitiveTopology)
    {
        // check extension
//        cmdHandle.setPrimitiveTopologyEXT(EnumCast<PrimitiveTopologyType, vk::PrimitiveTopology>(primitiveTopology)
    }

    void VulkanRasterPassCommandRecorder::SetBlendConstant(const float *inConstants)
    {
        vkCmdSetBlendConstants(nativeCmdBuffer, inConstants);
    }

    void VulkanRasterPassCommandRecorder::SetStencilReference(uint32_t inReference)
    {
        // TODO stencil face;
        vkCmdSetStencilReference(nativeCmdBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, inReference);
    }

    void VulkanRasterPassCommandRecorder::EndPass()
    {
        auto* pfn = device.GetGpu().GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkCmdEndRenderingKHR>("vkCmdEndRenderingKHR");
        pfn(nativeCmdBuffer);
    }
}
