//
// Created by Zach Lee on 2022/6/4.
//

#include <RHI/Vulkan/CommandEncoder.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Pipeline.h>
#include <RHI/Vulkan/CommandBuffer.h>
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

    ComputePassCommandEncoder* VKCommandEncoder::BeginComputePass()
    {
        return nullptr;
    }

    GraphicsPassCommandEncoder* VKCommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new VKGraphicsPassCommandEncoder(device, commandBuffer, beginInfo);
    }

    void VKCommandEncoder::End()
    {
        commandBuffer.GetNativeHandle().end();
    }

    VKGraphicsPassCommandEncoder::VKGraphicsPassCommandEncoder(VKDevice& dev, VKCommandBuffer& cmd,
        const GraphicsPassBeginInfo* beginInfo) : device(dev), commandBuffer(cmd)
    {
        cmdHandle = commandBuffer.GetNativeHandle();

        vk::RenderPassBeginInfo passBegin = {};
//        commandBuffer.GetNativeHandle().beginRenderPass()
    }

    VKGraphicsPassCommandEncoder::~VKGraphicsPassCommandEncoder()
    {
    }

    void VKGraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* pipeline)
    {
        graphicsPipeline = static_cast<VKGraphicsPipeline*>(pipeline);
    }

    void VKGraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* bindGroup)
    {
    }

    void VKGraphicsPassCommandEncoder::SetIndexBuffer(BufferView *bufferView)
    {
//        cmdHandle.bindIndexBuffer()
    }

    void VKGraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView *bufferView)
    {
        // TODO gather slot buffer.
//        cmdHandle.bindVertexBuffers()
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