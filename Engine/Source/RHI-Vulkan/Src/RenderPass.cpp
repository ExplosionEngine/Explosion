//
// Created by Zach Lee on 2022/4/4.
//

#include <RHI/Vulkan/RenderPass.h>
#include <RHI/Vulkan/Device.h>
#include <RHI/Vulkan/Common.h>

namespace RHI::Vulkan {

    VKRenderPass::VKRenderPass(VKDevice& dev, const GraphicsPipelineCreateInfo* createInfo) : device(dev)
    {
        CreateNativeRenderPass(createInfo);
    }

    VKRenderPass::~VKRenderPass()
    {
        if (renderPass) {
            device.GetVkDevice().destroyRenderPass(renderPass, nullptr);
        }
    }

    void VKRenderPass::CreateNativeRenderPass(const GraphicsPipelineCreateInfo* createInfo)
    {
        uint32_t colorCount = createInfo->fragmentState.colorTargetNum;
        uint32_t colorWithResolve = createInfo->multiSampleState.count > 1 ? colorCount * 2 : colorCount;
        uint32_t dsCount = createInfo->depthStencilState.depthEnable ? 1 : 0;

        std::vector<vk::AttachmentReference> colors(colorCount);
        std::vector<vk::AttachmentReference> resolves(colorWithResolve - colorCount);
        vk::AttachmentReference dsRef;

        std::vector<vk::AttachmentDescription> attachments(colorWithResolve + dsCount);
        for (uint32_t i = 0; i < colorCount; ++i) {
            auto colorState = createInfo->fragmentState.colorTargets[i];

            vk::AttachmentDescription& desc = attachments[i];
            desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(colorState.format))
                .setSamples(vk::SampleCountFlagBits::e1)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

            vk::AttachmentReference& colorRef = colors[i];
            colorRef.setAttachment(i).setLayout(vk::ImageLayout::eColorAttachmentOptimal);

            if (createInfo->multiSampleState.count > 1) {
                attachments[colorCount + i] = desc;
                attachments[colorCount + i].setSamples(static_cast<vk::SampleCountFlagBits>(createInfo->multiSampleState.count));
                vk::AttachmentReference& resolveRef = resolves[i];
                resolveRef.setAttachment(i).setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
            }
        }

        if (dsCount > 0) {
            vk::AttachmentDescription& desc = attachments[colorWithResolve];
            desc.setFormat(VKEnumCast<PixelFormat, vk::Format>(createInfo->depthStencilState.format))
                .setSamples(static_cast<vk::SampleCountFlagBits>(createInfo->multiSampleState.count))
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setFinalLayout(vk::ImageLayout::eDepthAttachmentOptimal)
                .setLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);

            dsRef.setAttachment(colorWithResolve).setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
        }

        vk::SubpassDescription subPassDesc = {};
        subPassDesc.setColorAttachments(colors)
            .setResolveAttachments(resolves)
            .setPDepthStencilAttachment(dsCount > 0 ? &dsRef : nullptr)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

        vk::RenderPassCreateInfo passInfo = {};
        passInfo.setAttachments(attachments)
            .setSubpassCount(1)
            .setPSubpasses(&subPassDesc);

        Assert(device.GetVkDevice().createRenderPass(&passInfo, nullptr, &renderPass) == vk::Result::eSuccess);
    }

}