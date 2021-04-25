//
// Created by John Kindem on 2021/4/25.
//

#include <Explosion/Driver/RenderPass.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/EnumAdapter.h>

namespace Explosion {
    RenderPass::RenderPass(Driver& driver, const Config& config)
        : driver(driver), device(*driver.GetDevice()), config(config)
    {
        CreateRenderPass();
    }

    RenderPass::~RenderPass()
    {
        DestroyRenderPass();
    }

    const VkRenderPass& RenderPass::GetVkRenderPass()
    {
        return vkRenderPass;
    }

    void RenderPass::CreateRenderPass()
    {
        std::vector<VkAttachmentDescription> attachmentDescriptions(config.attachmentConfigs.size());
        for (uint32_t i = 0; i < attachmentDescriptions.size(); i++) {
            auto& attachmentConfig = config.attachmentConfigs[i];
            attachmentDescriptions[i].flags = 0;
            attachmentDescriptions[i].format = VkConvert<Format, VkFormat>(attachmentConfig.format);
            attachmentDescriptions[i].samples = VK_SAMPLE_COUNT_1_BIT;
            attachmentDescriptions[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            if (attachmentConfig.type == AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT) {
                attachmentDescriptions[i].loadOp = VkConvert<AttachmentLoadOp, VkAttachmentLoadOp>(attachmentConfig.loadOp);
                attachmentDescriptions[i].storeOp = VkConvert<AttachmentStoreOp, VkAttachmentStoreOp>(attachmentConfig.storeOp);
                attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            } else if (attachmentConfig.type == AttachmentType::COLOR_ATTACHMENT) {
                attachmentDescriptions[i].loadOp = VkConvert<AttachmentLoadOp, VkAttachmentLoadOp>(attachmentConfig.loadOp);
                attachmentDescriptions[i].storeOp = VkConvert<AttachmentStoreOp, VkAttachmentStoreOp>(attachmentConfig.storeOp);
                attachmentDescriptions[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescriptions[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            } else {
                attachmentDescriptions[i].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachmentDescriptions[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachmentDescriptions[i].stencilLoadOp = VkConvert<AttachmentLoadOp, VkAttachmentLoadOp>(attachmentConfig.loadOp);
                attachmentDescriptions[i].stencilStoreOp = VkConvert<AttachmentStoreOp, VkAttachmentStoreOp>(attachmentConfig.storeOp);
                attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
        }

        std::vector<VkAttachmentReference> colorAttachmentReferences;
        std::vector<VkAttachmentReference> depthStencilAttachmentReferences;
        for (uint32_t i = 0; i < config.attachmentConfigs.size(); i++) {
            auto& attachmentConfig = config.attachmentConfigs[i];
            if (attachmentConfig.type == AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT
                || attachmentConfig.type == AttachmentType::COLOR_ATTACHMENT) {
                colorAttachmentReferences.emplace_back(VkAttachmentReference { i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
            } else {
                depthStencilAttachmentReferences.emplace_back(VkAttachmentReference {i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
            }
        }
        if (depthStencilAttachmentReferences.size() > 1) {
            throw std::runtime_error("num of depth stencil attachment is upper than 1");
        }

        // sub pass
        VkSubpassDescription subPassDescription {};
        subPassDescription.flags = 0;
        subPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPassDescription.inputAttachmentCount = 0;
        subPassDescription.pInputAttachments = nullptr;
        subPassDescription.colorAttachmentCount = colorAttachmentReferences.size();
        subPassDescription.pColorAttachments = colorAttachmentReferences.data();
        subPassDescription.pDepthStencilAttachment = !depthStencilAttachmentReferences.empty() ? depthStencilAttachmentReferences.data() : nullptr;
        subPassDescription.pResolveAttachments = nullptr;
        subPassDescription.preserveAttachmentCount = 0;
        subPassDescription.pResolveAttachments = nullptr;

        // create
        VkRenderPassCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.attachmentCount = attachmentDescriptions.size();
        createInfo.pAttachments = attachmentDescriptions.data();
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subPassDescription;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = nullptr;

        if (vkCreateRenderPass(device.GetVkDevice(), &createInfo, nullptr, &vkRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan render pass");
        }
    }

    void RenderPass::DestroyRenderPass()
    {
        vkDestroyRenderPass(device.GetVkDevice(), vkRenderPass, nullptr);
    }
}
