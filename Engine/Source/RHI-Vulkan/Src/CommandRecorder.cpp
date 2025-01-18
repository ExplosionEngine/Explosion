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
    static VkAccessFlags GetBufferMemoryBarrierAccessFlags(const BufferState inState)
    {
        static std::unordered_map<BufferState, VkAccessFlags> map = {
            { BufferState::undefined, VK_ACCESS_NONE },
            { BufferState::staging, VK_ACCESS_HOST_WRITE_BIT },
            { BufferState::copySrc, VK_ACCESS_TRANSFER_READ_BIT },
            { BufferState::copyDst, VK_ACCESS_TRANSFER_WRITE_BIT },
            { BufferState::shaderReadOnly, VK_ACCESS_SHADER_READ_BIT },
            { BufferState::storage, VK_ACCESS_SHADER_READ_BIT },
            { BufferState::rwStorage, VK_ACCESS_SHADER_WRITE_BIT }
        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetBufferPipelineBarrierSrcStage(const BufferState inState)
    {
        static std::unordered_map<BufferState, VkPipelineStageFlags> map = {
            { BufferState::undefined, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },
            { BufferState::staging, VK_PIPELINE_STAGE_HOST_BIT },
            { BufferState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::rwStorage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }

        };
        return map.at(inState);
    }

    static VkPipelineStageFlags GetBufferPipelineBarrierDstStage(const BufferState inState)
    {
        static std::unordered_map<BufferState, VkPipelineStageFlags> map = {
            { BufferState::undefined, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT },
            { BufferState::staging, VK_PIPELINE_STAGE_HOST_BIT },
            { BufferState::copySrc, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::copyDst, VK_PIPELINE_STAGE_TRANSFER_BIT },
            { BufferState::shaderReadOnly, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::storage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },
            { BufferState::rwStorage, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT }
        };
        return map.at(inState);
    }

    static VkAccessFlags GetTextureMemoryBarrierAccessFlags(const TextureState inState)
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

    static VkPipelineStageFlags GetTexturePipelineBarrierSrcStage(const TextureState inState)
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

    static VkPipelineStageFlags GetTexturePipelineBarrierDstStage(const TextureState inState)
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

    static VkImageLayout GetTextureLayout(const TextureState inState)
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

    static VkImageSubresourceLayers GetNativeImageSubResourceLayers(const TextureSubResourceInfo& subResourceInfo)
    {
        VkImageSubresourceLayers result {};
        result.mipLevel = subResourceInfo.mipLevel;
        result.baseArrayLayer = subResourceInfo.arrayLayer;
        result.layerCount = 1;
        result.aspectMask = EnumCast<TextureAspect, VkImageAspectFlags>(subResourceInfo.aspect);
        return result;
    }

    static VkBufferImageCopy GetNativeBufferImageCopy(Device& device, const Texture& texture, const BufferTextureCopyInfo& copyInfo)
    {
        const auto aspectLayout = device.GetTextureSubResourceCopyFootprint(texture, copyInfo.textureSubResource); // NOLINT
        const auto createInfo = texture.GetCreateInfo();

        const auto linearRowPitch = GetBytesPerPixel(createInfo.format) * copyInfo.copyRegion.x;
        const auto linearSlicePitch = linearRowPitch * copyInfo.copyRegion.y;

        VkBufferImageCopy result {};
        result.bufferOffset = copyInfo.bufferOffset;
        result.bufferRowLength = aspectLayout.rowPitch == linearRowPitch ? 0 : aspectLayout.rowPitch;
        result.bufferImageHeight = aspectLayout.slicePitch == linearSlicePitch ? 0 : aspectLayout.slicePitch;
        result.imageOffset = { static_cast<int32_t>(copyInfo.textureOrigin.x), static_cast<int32_t>(copyInfo.textureOrigin.y), static_cast<int32_t>(copyInfo.textureOrigin.z) };
        result.imageExtent = { copyInfo.copyRegion.x, copyInfo.copyRegion.y, copyInfo.copyRegion.z };
        result.imageSubresource = GetNativeImageSubResourceLayers(copyInfo.textureSubResource);
        return result;
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
            const auto* nativeBuffer = static_cast<VulkanBuffer*>(bufferBarrierInfo.pointer);

            VkBufferMemoryBarrier bufferBarrier {};
            bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            bufferBarrier.buffer = nativeBuffer->GetNative();
            bufferBarrier.size = nativeBuffer->GetCreateInfo().size;
            bufferBarrier.offset = 0;
            bufferBarrier.srcAccessMask = GetBufferMemoryBarrierAccessFlags(bufferBarrierInfo.before);
            bufferBarrier.dstAccessMask = GetBufferMemoryBarrierAccessFlags(bufferBarrierInfo.after);

            vkCmdPipelineBarrier(
                commandBuffer.GetNative(),
                GetBufferPipelineBarrierSrcStage(bufferBarrierInfo.before), GetBufferPipelineBarrierDstStage(bufferBarrierInfo.after),
                VK_DEPENDENCY_BY_REGION_BIT,
                0, nullptr,
                1, &bufferBarrier,
                0, nullptr);
        } else if (inBarrier.type == ResourceType::texture) {
            const auto& textureBarrierInfo = inBarrier.texture;

            const auto* nativeTexture = static_cast<VulkanTexture*>(textureBarrierInfo.pointer);
            VkImageMemoryBarrier imageBarrier {};
            imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageBarrier.image = nativeTexture->GetNative();
            imageBarrier.oldLayout = GetTextureLayout(textureBarrierInfo.before);
            imageBarrier.srcAccessMask = GetTextureMemoryBarrierAccessFlags(textureBarrierInfo.before);
            imageBarrier.newLayout = GetTextureLayout(textureBarrierInfo.after);
            imageBarrier.dstAccessMask = GetTextureMemoryBarrierAccessFlags(textureBarrierInfo.after);
            imageBarrier.subresourceRange = nativeTexture->GetNativeSubResourceFullRange();

            vkCmdPipelineBarrier(
                commandBuffer.GetNative(),
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
        vkEndCommandBuffer(commandBuffer.GetNative());
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

    void VulkanCopyPassCommandRecorder::CopyBufferToBuffer(Buffer* src, Buffer* dst, const BufferCopyInfo& copyInfo)
    {
        const auto* srcBuffer = static_cast<VulkanBuffer*>(src);
        const auto* dstBuffer = static_cast<VulkanBuffer*>(dst);

        VkBufferCopy nativeBufferCopy {};
        nativeBufferCopy.srcOffset = copyInfo.srcOffset;
        nativeBufferCopy.dstOffset = copyInfo.dstOffset;
        nativeBufferCopy.size = copyInfo.copySize;

        vkCmdCopyBuffer(commandBuffer.GetNative(), srcBuffer->GetNative(), dstBuffer->GetNative(), 1, &nativeBufferCopy);
    }

    void VulkanCopyPassCommandRecorder::CopyBufferToTexture(Buffer* src, Texture* dst, const BufferTextureCopyInfo& copyInfo)
    {
        const auto* srcBuffer = static_cast<VulkanBuffer*>(src);
        const auto* dstTexture = static_cast<VulkanTexture*>(dst);

        const VkBufferImageCopy nativeBufferImageCopy = GetNativeBufferImageCopy(device, *dst, copyInfo);
        vkCmdCopyBufferToImage(commandBuffer.GetNative(), srcBuffer->GetNative(), dstTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &nativeBufferImageCopy);
    }

    void VulkanCopyPassCommandRecorder::CopyTextureToBuffer(Texture* src, Buffer* dst, const BufferTextureCopyInfo& copyInfo)
    {
        const auto* srcTexture = static_cast<VulkanTexture*>(src);
        const auto* dstBuffer = static_cast<VulkanBuffer*>(dst);

        const VkBufferImageCopy nativeBufferImageCopy = GetNativeBufferImageCopy(device, *src, copyInfo);
        vkCmdCopyImageToBuffer(commandBuffer.GetNative(), srcTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer->GetNative(), 1, &nativeBufferImageCopy);
    }

    void VulkanCopyPassCommandRecorder::CopyTextureToTexture(Texture* src, Texture* dst, const TextureCopyInfo& copyInfo)
    {
        const auto* srcTexture = static_cast<VulkanTexture*>(src);
        const auto* dstTexture = static_cast<VulkanTexture*>(dst);

        VkImageCopy nativeImageCopy {};
        nativeImageCopy.srcSubresource = GetNativeImageSubResourceLayers(copyInfo.srcSubResource);
        nativeImageCopy.srcOffset = { static_cast<int32_t>(copyInfo.srcOrigin.x), static_cast<int32_t>(copyInfo.srcOrigin.y), static_cast<int32_t>(copyInfo.srcOrigin.z) };
        nativeImageCopy.dstSubresource = GetNativeImageSubResourceLayers(copyInfo.dstSubResource);
        nativeImageCopy.dstOffset = { static_cast<int32_t>(copyInfo.dstOrigin.x), static_cast<int32_t>(copyInfo.dstOrigin.y), static_cast<int32_t>(copyInfo.dstOrigin.z) };
        nativeImageCopy.extent = { copyInfo.copyRegion.x, copyInfo.copyRegion.y, copyInfo.copyRegion.z };

        vkCmdCopyImage(commandBuffer.GetNative(), srcTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstTexture->GetNative(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &nativeImageCopy);
    }

    void VulkanCopyPassCommandRecorder::EndPass()
    {
    }

    VulkanComputePassCommandRecorder::VulkanComputePassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
        , computePipeline(nullptr)
    {
    }

    VulkanComputePassCommandRecorder::~VulkanComputePassCommandRecorder() = default;

    void VulkanComputePassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void VulkanComputePassCommandRecorder::SetPipeline(ComputePipeline* inPipeline)
    {
        computePipeline = static_cast<VulkanComputePipeline*>(inPipeline);
        Assert(computePipeline);

        vkCmdBindPipeline(commandBuffer.GetNative(), VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline->GetNative());
    }

    void VulkanComputePassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        auto* vBindGroup = static_cast<VulkanBindGroup*>(inBindGroup);
        VkDescriptorSet descriptorSet = vBindGroup->GetNative();
        VkPipelineLayout layout = computePipeline->GetPipelineLayout()->GetNative();

        vkCmdBindDescriptorSets(commandBuffer.GetNative(), VK_PIPELINE_BIND_POINT_COMPUTE, layout, inLayoutIndex, 1, &descriptorSet, 0, nullptr);
    }

    void VulkanComputePassCommandRecorder::Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ)
    {
        vkCmdDispatch(commandBuffer.GetNative(), inGroupCountX, inGroupCountY, inGroupCountZ);
    }

    void VulkanComputePassCommandRecorder::EndPass()
    {

    }

    VulkanRasterPassCommandRecorder::VulkanRasterPassCommandRecorder(VulkanDevice& inDevice, VulkanCommandRecorder& inCmdRecorder, VulkanCommandBuffer& inCmdBuffer, const RasterPassBeginInfo& inBeginInfo)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
        , rasterPipeline(nullptr)
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
        const auto& textureCreateInfo = textureView->GetTexture().GetCreateInfo();

        VkRenderingInfoKHR renderingInfo = {};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.colorAttachmentCount = colorAttachmentInfos.size();
        renderingInfo.pColorAttachments = colorAttachmentInfos.data();
        renderingInfo.layerCount = textureView->GetArrayLayerNum();
        renderingInfo.renderArea = {{0, 0}, {static_cast<uint32_t>(textureCreateInfo.width), static_cast<uint32_t>(textureCreateInfo.height)}};
        renderingInfo.viewMask = 0;

        if (inBeginInfo.depthStencilAttachment.has_value())
        {
            const auto* depthStencilTextureView = static_cast<VulkanTextureView*>(inBeginInfo.depthStencilAttachment->view);

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

        auto* pfn = device.GetGpu().GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkCmdBeginRenderingKHR>("vkCmdBeginRenderingKHR");
        pfn(commandBuffer.GetNative(), &renderingInfo);
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

       vkCmdBindPipeline(commandBuffer.GetNative(), VK_PIPELINE_BIND_POINT_GRAPHICS, rasterPipeline->GetNative());
    }

    void VulkanRasterPassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        const auto* vBindGroup = static_cast<VulkanBindGroup*>(inBindGroup);
        const VkDescriptorSet descriptorSet = vBindGroup->GetNative();
        const VkPipelineLayout layout = rasterPipeline->GetPipelineLayout()->GetNative();

        vkCmdBindDescriptorSets(commandBuffer.GetNative(), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, inLayoutIndex, 1, &descriptorSet, 0, nullptr);
    }

    void VulkanRasterPassCommandRecorder::SetIndexBuffer(BufferView *inBufferView)
    {
        const auto* mBufferView = static_cast<VulkanBufferView*>(inBufferView);

        const VkBuffer indexBuffer = mBufferView->GetBuffer().GetNative();
        const auto vkFormat = EnumCast<IndexFormat, VkIndexType>(mBufferView->GetIndexFormat());

        vkCmdBindIndexBuffer(commandBuffer.GetNative(), indexBuffer, 0, vkFormat);
    }

    void VulkanRasterPassCommandRecorder::SetVertexBuffer(size_t inSlot, BufferView *inBufferView)
    {
        const auto* mBufferView = static_cast<VulkanBufferView*>(inBufferView);

        const VkBuffer vertexBuffer = mBufferView->GetBuffer().GetNative();
        const VkDeviceSize offset[] = { mBufferView->GetOffset() };
        vkCmdBindVertexBuffers(commandBuffer.GetNative(), inSlot, 1, &vertexBuffer, offset);
    }

    void VulkanRasterPassCommandRecorder::Draw(const size_t inVertexCount, const size_t inInstanceCount, const size_t inFirstVertex, const size_t inFirstInstance)
    {
        vkCmdDraw(commandBuffer.GetNative(), inVertexCount, inInstanceCount, inFirstVertex, inFirstInstance);
    }

    void VulkanRasterPassCommandRecorder::DrawIndexed(const size_t inIndexCount, const size_t inInstanceCount, const size_t inFirstIndex, const size_t inBaseVertex, const size_t inFirstInstance)
    {
        vkCmdDrawIndexed(commandBuffer.GetNative(), inIndexCount, inInstanceCount, inFirstIndex, inBaseVertex, inFirstInstance);
    }

    void VulkanRasterPassCommandRecorder::SetViewport(const float inX, const float inY, const float inWidth, const float inHeight, const float inMinDepth, const float inMaxDepth)
    {
        VkViewport viewport{};
        viewport.x = inX;
        viewport.y = inY;
        viewport.width = inWidth;
        viewport.height = inHeight;
        viewport.minDepth = inMinDepth;
        viewport.maxDepth = inMaxDepth;
        vkCmdSetViewport(commandBuffer.GetNative(), 0, 1, &viewport);
    }

    void VulkanRasterPassCommandRecorder::SetScissor(const uint32_t inLeft, const uint32_t inTop, const uint32_t inRight, const uint32_t inBottom)
    {
        VkRect2D rect;
        rect.offset = {static_cast<int32_t>(inLeft), static_cast<int32_t>(inTop) };
        rect.extent = {inRight - inLeft, inBottom - inTop };
        vkCmdSetScissor(commandBuffer.GetNative(), 0, 1, &rect);
    }

    void VulkanRasterPassCommandRecorder::SetPrimitiveTopology(PrimitiveTopology inPrimitiveTopology)
    {
#if PLATFORM_MACOS
        // MoltenVK not support use vkCmdSetPrimitiveTopology() directly current
        auto* pfn = device.GetGpu().GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkCmdSetPrimitiveTopologyEXT>("vkCmdSetPrimitiveTopologyEXT");
        pfn(commandBuffer.GetNative(), EnumCast<PrimitiveTopology, VkPrimitiveTopology>(inPrimitiveTopology));
#else
        vkCmdSetPrimitiveTopology(commandBuffer.GetNative(), EnumCast<PrimitiveTopology, VkPrimitiveTopology>(inPrimitiveTopology));
#endif
    }

    void VulkanRasterPassCommandRecorder::SetBlendConstant(const float *inConstants)
    {
        vkCmdSetBlendConstants(commandBuffer.GetNative(), inConstants);
    }

    void VulkanRasterPassCommandRecorder::SetStencilReference(const uint32_t inReference)
    {
        // TODO stencil face;
        vkCmdSetStencilReference(commandBuffer.GetNative(), VK_STENCIL_FACE_FRONT_AND_BACK, inReference);
    }

    void VulkanRasterPassCommandRecorder::EndPass()
    {
        auto* pfn = device.GetGpu().GetInstance().FindOrGetTypedDynamicFuncPointer<PFN_vkCmdEndRenderingKHR>("vkCmdEndRenderingKHR");
        pfn(commandBuffer.GetNative());
    }
}
