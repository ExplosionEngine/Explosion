//
// Created by John Kindem on 2021/4/25.
//

#include <stdexcept>
#include <utility>

#include <Explosion/RHI/FrameBuffer.h>
#include <Explosion/RHI/Driver.h>
#include <Explosion/RHI/ImageView.h>
#include <Explosion/RHI/RenderPass.h>

namespace Explosion::RHI {
    FrameBuffer::FrameBuffer(Driver& driver, FrameBuffer::Config config)
        : driver(driver), device(*driver.GetDevice()), config(std::move(config))
    {
        CreateFrameBuffer();
    }

    FrameBuffer::~FrameBuffer()
    {
        DestroyFrameBuffer();
    }

    const VkFramebuffer& FrameBuffer::GetVkFrameBuffer()
    {
        return vkFramebuffer;
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
        createInfo.renderPass = config.renderPass->GetVkRenderPass();
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
