//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandEncoder.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/CommandBuffer.h>
#include <RHI/Vulkan/Buffer.h>
#include <RHI/Vulkan/BufferView.h>
#include <RHI/Vulkan/TextureView.h>
#include <RHI/Vulkan/Common.h>

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
    }

    void VKCommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
    }

    void VKCommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
    }

    void VKCommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo,
        Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo, const Extent<3>& size)
    {
    }

    void VKCommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
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

    VKGraphicsPassCommandEncoder::VKGraphicsPassCommandEncoder(VKDevice& dev, VKCommandBuffer& cmd,
        const GraphicsPassBeginInfo* beginInfo) : device(dev), commandBuffer(cmd)
    {
        cmdHandle = commandBuffer.GetVkCommandBuffer();
        auto pipeline = dynamic_cast<VKGraphicsPipeline*>(beginInfo->pipeline);

        auto textureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[0].view);
        std::array<vk::ImageView, 1> attachments = {textureView->GetVkImageView()};

        vk::FramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.setRenderPass(pipeline->GetVkRenderPass())
            .setAttachmentCount(1)
            .setPAttachments(attachments.data())
            .setLayers(1)
            .setWidth(1024)
            .setHeight(768);

        vk::Framebuffer framebuffer;
        Assert(device.GetVkDevice().createFramebuffer(&framebufferInfo, nullptr, &framebuffer) == vk::Result::eSuccess);

        auto color = beginInfo->colorAttachments[0].clearValue;
        vk::ClearColorValue colorValue = std::array<float, 4> {color.r, color.g, color.b, color.a};
        std::array<vk::ClearValue, 1> clearValue = {colorValue};

        vk::RenderPassBeginInfo passBegin = {};
        passBegin.setRenderPass(pipeline->GetVkRenderPass())
            .setFramebuffer(framebuffer)
            .setClearValueCount(clearValue.size())
            .setPClearValues(clearValue.data());

        cmdHandle.beginRenderPass(&passBegin, vk::SubpassContents::eInline);
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
        IndexFormat format = mBufferView->GetIndexFormat();
        vk::IndexType vkFormat;
        if (format == IndexFormat::UINT16) {
            vkFormat = vk::IndexType::eUint16;
        } else if (format == IndexFormat::UINT32) {
            vkFormat = vk::IndexType::eUint32;
        }

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
        cmdHandle.endRenderPass();
        delete this;
    }
}