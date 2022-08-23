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
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    static vk::IndexType GetVkIndexFormat(IndexFormat format)
    {
        static std::unordered_map<IndexFormat, vk::IndexType> rules = {
            {IndexFormat::UINT16,vk::IndexType::eUint16},
            {IndexFormat::UINT32,vk::IndexType::eUint32}
        };

        vk::IndexType result = {};
        for (const auto& rule : rules) {
            if (format & rule.first) {
                result = rule.second;
                break;
            }
        }
        return result;
    }

    static vk::AttachmentLoadOp GetVkLoadOp(LoadOp option)
    {
        if (option == LoadOp::LOAD) {
            return vk::AttachmentLoadOp::eLoad;
        } else if (option == LoadOp::CLEAR) {
            return vk::AttachmentLoadOp::eClear;
        } else {
            return vk::AttachmentLoadOp::eNoneEXT;
        }
    }

    static vk::AttachmentStoreOp GetVkStoreOp(StoreOp option)
    {
        if (option == StoreOp::STORE) {
            return vk::AttachmentStoreOp::eStore;
        } else if (option == StoreOp::DISCARD) {
            //VK_ATTACHMENT_STORE_OP_DONT_CARE specifies the contents within the render area are not needed after rendering,
            // and may be discarded;
            return vk::AttachmentStoreOp::eDontCare;
        } else {
            return vk::AttachmentStoreOp::eNoneEXT;
        }
    }

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
        dynamicLoader = vk::DispatchLoaderDynamic(dev.GetGpu()->GetVKInstance(), vkGetInstanceProcAddr, dev.GetVkDevice());
        dynamicLoader.vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(dev.GetGpu()->GetVKInstance().getProcAddr("vkCmdBeginRenderingKHR"));
        dynamicLoader.vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(dev.GetGpu()->GetVKInstance().getProcAddr("vkCmdEndRenderingKHR"));

        std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos(beginInfo->colorAttachmentNum);
        for (size_t i = 0; i < beginInfo->colorAttachmentNum; i++)
        {
            //TODO
            //How to cast ColorNormalized to vk::ClearColorValue more simpler?
            auto value = beginInfo->colorAttachments[i].clearValue;
            std::array<float, 4> colorValue = {value.a, value.b, value.g, value.r};
            vk::ClearValue clearValue;
            clearValue.setColor(colorValue);
            auto* colorTextureView = dynamic_cast<VKTextureView*>(beginInfo->colorAttachments[i].view);

            colorAttachmentInfos[i].setImageView(colorTextureView->GetVkImageView())
                //TODO
                //imageLayout is the layout that imageView will be in during rendering.
                //Is the value of imageLayout fixd to VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR
                .setImageLayout(vk::ImageLayout::eAttachmentOptimalKHR)
                .setLoadOp(GetVkLoadOp(beginInfo->colorAttachments[i].loadOp))
                .setStoreOp(GetVkStoreOp(beginInfo->colorAttachments[i].storeOp))
                .setClearValue(clearValue);
        }

        auto width = beginInfo->colorAttachments[0].width;
        auto height = beginInfo->colorAttachments[0].height;
        vk::RenderingInfoKHR renderingInfo;
        renderingInfo.setColorAttachmentCount(colorAttachmentInfos.size())
            .setPColorAttachments(colorAttachmentInfos.data())
            // layerCount is the number of layers rendered to in each attachment when viewMask is 0.
            .setLayerCount(1)
            .setRenderArea({{0, 0}, {width, height}});

        if (beginInfo->depthStencilAttachment != nullptr)
        {
            vk::ClearValue clearValue;
            clearValue.setDepthStencil({beginInfo->depthStencilAttachment->depthClearValue, beginInfo->depthStencilAttachment->stencilClearValue});
            auto* depthStencilTextureView = dynamic_cast<VKTextureView*>(beginInfo->depthStencilAttachment->view);

            //TODO
            //A single depth stencil attachment info can be used, they can also be specified separately.
            //Depth and stencil have their own loadOp and storeOp separately
            vk::RenderingAttachmentInfo depthStencilAttachmentInfo;
            depthStencilAttachmentInfo.setImageView(depthStencilTextureView->GetVkImageView())
                .setImageLayout(vk::ImageLayout::eDepthAttachmentOptimalKHR) // TODO as color attachment above
                .setLoadOp(GetVkLoadOp(beginInfo->depthStencilAttachment->depthLoadOp))
                .setStoreOp(GetVkStoreOp(beginInfo->depthStencilAttachment->depthStoreOp))
                .setClearValue(clearValue);

            renderingInfo.setPDepthAttachment(&depthStencilAttachmentInfo)
                .setPStencilAttachment(&depthStencilAttachmentInfo);
        }

        cmdHandle = cmd.GetVkCommandBuffer();
        cmdHandle.beginRenderingKHR(&renderingInfo, dynamicLoader);
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
        auto vkFormat = GetVkIndexFormat(mBufferView->GetIndexFormat());

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
        cmdHandle.endRenderingKHR(dynamicLoader);
        delete this;
    }
}