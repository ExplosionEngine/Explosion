//
// Created by John Kindem on 2021/4/25.
//

#include <Explosion/Driver/FrameBuffer.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/ImageView.h>
#include <Explosion/Driver/RenderPass.h>

namespace Explosion {
    FrameBuffer::FrameBuffer(Driver& driver, RenderPass* renderPass, const FrameBuffer::Config& config)
        : driver(driver), device(*driver.GetDevice()), renderPass(renderPass), config(config)
    {
        CreateFrameBuffer();
    }

    FrameBuffer::~FrameBuffer()
    {
        DestroyFrameBuffer();
    }

    void FrameBuffer::CreateFrameBuffer()
    {
        std::vector<VkImageView> attachments(config.attachments.size());
        for (uint32_t i = 0; i < attachments.size(); i++) {
            attachments[i] = config.attachments[i]->GetVkImageView();
        }

        VkFramebufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = renderPass->GetVkRenderPass();
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = config.width;
        createInfo.height = config.height;
        createInfo.layers = config.layers;

        if (vkCreateFramebuffer(device.GetVkDevice(), &createInfo, nullptr, &vkFramebuffer) != VK_NULL_HANDLE) {
            throw std::runtime_error("failed to create vulkan frame buffer");
        }
    }

    void FrameBuffer::DestroyFrameBuffer()
    {
        vkDestroyFramebuffer(device.GetVkDevice(), vkFramebuffer, nullptr);
    }
}
