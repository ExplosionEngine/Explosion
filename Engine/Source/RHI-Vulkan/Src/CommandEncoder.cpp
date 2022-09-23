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
#include <RHI/Synchronous.h>

namespace RHI::Vulkan {

    VKCommandEncoder::VKCommandEncoder(VKDevice& dev, VKCommandBuffer& cmd)
        : device(dev), commandBuffer(cmd)
    {
    }
    VKCommandEncoder::~VKCommandEncoder()
    {
    }

    void VKCommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
        auto* srcBuffer = dynamic_cast<VKBuffer*>(src);
        auto* dstBuffer = dynamic_cast<VKBuffer*>(dst);

        vk::BufferCopy copyRegion {};
        copyRegion.setSrcOffset(srcOffset)
            .setDstOffset(dstOffset)
            .setSrcOffset(size);
        commandBuffer.GetVkCommandBuffer().copyBuffer(srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &copyRegion);
    }

    void VKCommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        auto* buffer = dynamic_cast<VKBuffer*>(src);
        auto* texture = dynamic_cast<VKTexture*>(dst);

        vk::BufferImageCopy copyRegion {};
        copyRegion.setImageExtent(vk::Extent3D(size.x, size.y, size.z))
            .setImageSubresource(vk::ImageSubresourceLayers(
                vk::ImageAspectFlags(GetAspectMask(subResourceInfo->aspect)),
                subResourceInfo->mipLevel,
                subResourceInfo->baseArrayLayer,
                subResourceInfo->arrayLayerNum));

        commandBuffer.GetVkCommandBuffer().copyBufferToImage(buffer->GetVkBuffer(), texture->GetImage(), vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);
    }

    void VKCommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        auto* buffer = dynamic_cast<VKBuffer*>(dst);
        auto* texture = dynamic_cast<VKTexture*>(src);

        vk::BufferImageCopy copyRegion {};
        copyRegion.setImageExtent(vk::Extent3D(size.x, size.y, size.z))
            .setImageSubresource(vk::ImageSubresourceLayers(
                vk::ImageAspectFlags(GetAspectMask(subResourceInfo->aspect)),
                subResourceInfo->mipLevel,
                subResourceInfo->baseArrayLayer,
                subResourceInfo->arrayLayerNum));

        commandBuffer.GetVkCommandBuffer().copyImageToBuffer(texture->GetImage(), vk::ImageLayout::eTransferSrcOptimal, buffer->GetVkBuffer(), 1, &copyRegion);
    }

    void VKCommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo,
        Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size)
    {
        auto* srcTexture = dynamic_cast<VKTexture*>(src);
        auto* dstTexture = dynamic_cast<VKTexture*>(dst);

        vk::ImageCopy copyRegion {};
        copyRegion.setExtent(vk::Extent3D(size.x, size.y, size.z))
            .setSrcSubresource(vk::ImageSubresourceLayers(
                vk::ImageAspectFlags(GetAspectMask(srcSubResourceInfo->aspect)),
                srcSubResourceInfo->mipLevel,
                srcSubResourceInfo->baseArrayLayer,
                srcSubResourceInfo->arrayLayerNum))
            .setDstSubresource(vk::ImageSubresourceLayers(
                vk::ImageAspectFlags(GetAspectMask(dstSubResourceInfo->aspect)),
                dstSubResourceInfo->mipLevel,
                dstSubResourceInfo->baseArrayLayer,
                dstSubResourceInfo->arrayLayerNum));

        commandBuffer.GetVkCommandBuffer().copyImage(srcTexture->GetImage(), vk::ImageLayout::eTransferSrcOptimal, dstTexture->GetImage(), vk::ImageLayout::eTransferDstOptimal, 1, &copyRegion);
    }

    static std::tuple<vk::ImageLayout, vk::AccessFlags, vk::PipelineStageFlags> GetBarrierInfo(TextureState status)
    {
        if (status == TextureState::PRESENT) {
            return {vk::ImageLayout::ePresentSrcKHR, vk::AccessFlagBits::eMemoryRead, vk::PipelineStageFlagBits::eBottomOfPipe};
        } else if (status == TextureState::RENDER_TARGET) {
            return {vk::ImageLayout::eColorAttachmentOptimal, vk::AccessFlagBits::eColorAttachmentWrite, vk::PipelineStageFlagBits::eColorAttachmentOutput};
        }
        return {vk::ImageLayout::eUndefined, vk::AccessFlags{}, vk::PipelineStageFlagBits::eTopOfPipe};
    }

    void VKCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        if (barrier.type == ResourceType::TEXTURE) {
            auto& textureBarrierInfo = barrier.texture;
            auto oldLayout = GetBarrierInfo(textureBarrierInfo.before == TextureState::PRESENT ? TextureState::UNDEFINED : textureBarrierInfo.before);
            auto newLayout = GetBarrierInfo(textureBarrierInfo.after);

            auto vkTexture = static_cast<VKTexture*>(textureBarrierInfo.pointer);
            vk::ImageMemoryBarrier imageBarrier = {};
            imageBarrier.setImage(vkTexture->GetImage())
                .setOldLayout(std::get<0>(oldLayout))
                .setSrcAccessMask(std::get<1>(oldLayout))
                .setNewLayout(std::get<0>(newLayout))
                .setDstAccessMask(std::get<1>(newLayout))
                .setSubresourceRange(vkTexture->GetRange(vk::ImageAspectFlagBits::eColor));
            commandBuffer.GetVkCommandBuffer().pipelineBarrier(std::get<2>(oldLayout), std::get<2>(newLayout), vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &imageBarrier);
        }
    }

    ComputePassCommandEncoder* VKCommandEncoder::BeginComputePass(const ComputePassBeginInfo* beginInfo)
    {
        return nullptr;
    }

    GraphicsPassCommandEncoder* VKCommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new VKGraphicsPassCommandEncoder(device, commandBuffer, beginInfo);
    }

    void VKCommandEncoder::End()
    {
        commandBuffer.GetVkCommandBuffer().end();
    }

    void VKCommandEncoder::SwapChainSync(SwapChain* swapChain)
    {
        auto vkSwapChain = static_cast<VKSwapChain*>(swapChain);
        auto signal = vkSwapChain->GetImageSemaphore();
        commandBuffer.AddWaitSemaphore(signal, vk::PipelineStageFlagBits::eColorAttachmentOutput);
        vkSwapChain->AddWaitSemaphore(commandBuffer.GetSignalSemaphores()[0]);
    }

    VKGraphicsPassCommandEncoder::VKGraphicsPassCommandEncoder(VKDevice& dev, VKCommandBuffer& cmd,
        const GraphicsPassBeginInfo* beginInfo) : device(dev), commandBuffer(cmd)
    {
        std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(beginInfo->colorAttachmentNum);
        for (size_t i = 0; i < beginInfo->colorAttachmentNum; i++)
        {
            auto* colorTextureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[i].view);
            colorAttachmentInfos[i].setImageView(colorTextureView->GetVkImageView())
                .setImageLayout(vk::ImageLayout::eAttachmentOptimalKHR)
                .setLoadOp(VKEnumCast<LoadOp, vk::AttachmentLoadOp>(beginInfo->colorAttachments[i].loadOp))
                .setStoreOp(VKEnumCast<StoreOp, vk::AttachmentStoreOp>(beginInfo->colorAttachments[i].storeOp))
                .setClearValue(vk::ClearValue(std::array<float, 4>{
                    beginInfo->colorAttachments[i].clearValue.r,
                    beginInfo->colorAttachments[i].clearValue.g,
                    beginInfo->colorAttachments[i].clearValue.b,
                    beginInfo->colorAttachments[i].clearValue.a
                    }));
        }

        auto* textureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[0].view);
        vk::RenderingInfoKHR renderingInfo;
        renderingInfo.setColorAttachmentCount(colorAttachmentInfos.size())
            .setPColorAttachments(colorAttachmentInfos.data())
            .setLayerCount(textureView->GetArrayLayerNum())
            .setRenderArea({{0, 0}, {static_cast<uint32_t>(textureView->GetTexture().GetExtent().x), static_cast<uint32_t>(textureView->GetTexture().GetExtent().y)}});

        if (beginInfo->depthStencilAttachment != nullptr)
        {
            auto* depthStencilTextureView = dynamic_cast<VKTextureView*>(beginInfo->depthStencilAttachment->view);
            //TODO
            //A single depth stencil attachment info can be used, they can also be specified separately.
            //Depth and stencil have their own loadOp and storeOp separately
            vk::RenderingAttachmentInfo depthStencilAttachmentInfo;
            depthStencilAttachmentInfo.setImageView(depthStencilTextureView->GetVkImageView())
                .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimalKHR) // TODO as color attachment above
                .setLoadOp(VKEnumCast<LoadOp, vk::AttachmentLoadOp>(beginInfo->depthStencilAttachment->depthLoadOp))
                .setStoreOp(VKEnumCast<StoreOp, vk::AttachmentStoreOp>(beginInfo->depthStencilAttachment->depthStoreOp))
                .setClearValue(vk::ClearValue({
                    beginInfo->depthStencilAttachment->depthClearValue,
                    beginInfo->depthStencilAttachment->stencilClearValue}));

            renderingInfo.setPDepthAttachment(&depthStencilAttachmentInfo)
                .setPStencilAttachment(&depthStencilAttachmentInfo);
        }

        cmdHandle = cmd.GetVkCommandBuffer();
        cmdHandle.beginRenderingKHR(&renderingInfo, device.GetGpu().GetInstance().GetVkDispatch());

        auto* pipeline = dynamic_cast<VKGraphicsPipeline*>(beginInfo->pipeline);
        cmdHandle.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetVkPipeline());
    }

    VKGraphicsPassCommandEncoder::~VKGraphicsPassCommandEncoder()
    {
    }

    void VKGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
    }

    void VKGraphicsPassCommandEncoder::SetIndexBuffer(BufferView *bufferView)
    {
        auto mBufferView = dynamic_cast<VKBufferView*>(bufferView);

        vk::Buffer indexBuffer = mBufferView->GetBuffer().GetVkBuffer();
        auto vkFormat = VKEnumCast<IndexFormat, vk::IndexType>(mBufferView->GetIndexFormat());

        cmdHandle.bindIndexBuffer(indexBuffer, 0, vkFormat);
    }

    void VKGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView *bufferView)
    {
        auto mBufferView = dynamic_cast<VKBufferView*>(bufferView);

        vk::Buffer vertexBuffer = mBufferView->GetBuffer().GetVkBuffer();
        vk::DeviceSize offset[] = {mBufferView->GetOffset()};
        cmdHandle.bindVertexBuffers(slot, 1, &vertexBuffer, offset);
    }

    void VKGraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
        cmdHandle.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VKGraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
        cmdHandle.drawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
    }

    void VKGraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        vk::Viewport viewport;
        viewport.setX(x)
            .setY(y)
            .setWidth(width)
            .setHeight(height)
            .setMinDepth(minDepth)
            .setMaxDepth(maxDepth);
        cmdHandle.setViewport(0, 1, &viewport);
    }

    void VKGraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        vk::Rect2D rect;
        rect.setOffset(vk::Offset2D{static_cast<int32_t>(left), static_cast<int32_t>(top)})
            .setExtent(vk::Extent2D{right - left, bottom - top});
        cmdHandle.setScissor(0, 1, &rect);
    }

    void VKGraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
    {
        // check extension
//        cmdHandle.setPrimitiveTopologyEXT(VKEnumCast<PrimitiveTopologyType, vk::PrimitiveTopology>(primitiveTopology)
    }

    void VKGraphicsPassCommandEncoder::SetBlendConstant(const float *constants)
    {
        cmdHandle.setBlendConstants(constants);
    }

    void VKGraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
        // TODO stencil face;
        cmdHandle.setStencilReference(vk::StencilFaceFlagBits::eFrontAndBack, reference);
    }

    void VKGraphicsPassCommandEncoder::EndPass()
    {
        cmdHandle.endRenderingKHR(device.GetGpu().GetInstance().GetVkDispatch());
        delete this;
    }
}