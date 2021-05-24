//
// Created by John Kindem on 2021/4/25.
//

#include <stdexcept>
#include <utility>

#include <Explosion/RHI/Vulkan/VulkanFrameBuffer.h>
#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/RHI/Vulkan/VulkanImageView.h>
#include <Explosion/RHI/Vulkan/VulkanRenderPass.h>

namespace Explosion::RHI {
    VulkanFrameBuffer::VulkanFrameBuffer(VulkanDriver& driver, VulkanFrameBuffer::Config config)
        : FrameBuffer(std::move(config)), driver(driver), device(*driver.GetDevice())
    {
        CreateFrameBuffer();
    }

    VulkanFrameBuffer::~VulkanFrameBuffer()
    {
        DestroyFrameBuffer();
    }

    const VkFramebuffer& VulkanFrameBuffer::GetVkFrameBuffer()
    {
        return vkFramebuffer;
    }

    void VulkanFrameBuffer::CreateFrameBuffer()
    {
        std::vector<VkImageView> attachments(config.attachments.size());
        for (uint32_t i = 0; i < attachments.size(); i++) {
            attachments[i] = dynamic_cast<VulkanImageView*>(config.attachments[i])->GetVkImageView();
        }

        VkFramebufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = dynamic_cast<VulkanRenderPass*>(config.renderPass)->GetVkRenderPass();
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = config.width;
        createInfo.height = config.height;
        createInfo.layers = config.layers;

        if (vkCreateFramebuffer(device.GetVkDevice(), &createInfo, nullptr, &vkFramebuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan frame buffer");
        }
    }

    void VulkanFrameBuffer::DestroyFrameBuffer()
    {
        vkDestroyFramebuffer(device.GetVkDevice(), vkFramebuffer, nullptr);
    }
}
