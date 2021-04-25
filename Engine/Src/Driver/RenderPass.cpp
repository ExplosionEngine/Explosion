//
// Created by John Kindem on 2021/4/25.
//

#include <Explosion/Driver/RenderPass.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/Image.h>
#include <Explosion/Driver/ImageView.h>
#include <Explosion/Driver/EnumAdapter.h>

namespace {
    template <class Type, class ObjType>
    bool IsPointerOf(const ObjType& object)
    {
        return dynamic_cast<Type*>(object) != nullptr;
    }
}

namespace Explosion {
    RenderPass::RenderPass(Driver& driver, const Config& config)
        : driver(driver), device(*driver.GetDevice()), config(config)
    {
        ValidateAttachments();
        CreateRenderPass();
        CreateFrameBuffer();
    }

    RenderPass::~RenderPass()
    {
        DestroyFrameBuffer();
        DestroyRenderPass();
    }

    void RenderPass::ValidateAttachments()
    {
        for (auto& colorAttachment : config.colorAttachments) {
            if (!IsPointerOf<ColorAttachment>(colorAttachment->GetImage())) {
                throw std::runtime_error("the given attachments is not a color attachment");
            }
        }
        if (!IsPointerOf<DepthStencilAttachment>(config.depthStencilAttachment->GetImage())) {
            throw std::runtime_error("the given attachment is not a depth stencil attachment");
        }
    }

    void RenderPass::CreateRenderPass()
    {
        VkAttachmentDescription basicAttachmentDescription {};
        basicAttachmentDescription.flags = 0;
        basicAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        basicAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        basicAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        basicAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        basicAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        basicAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        // color attachments
        std::vector<VkAttachmentDescription> attachmentDescriptions(config.colorAttachments.size() + 1);
        for (uint32_t i = 0; i < config.colorAttachments.size(); i++) {
            attachmentDescriptions[i] = basicAttachmentDescription;

            auto* image = config.colorAttachments[i]->GetImage();
            attachmentDescriptions[i].format = VkConvert<Format, VkFormat>(image->GetConfig().format);

            auto* attachmentImage = dynamic_cast<ColorAttachment*>(image);
            if (attachmentImage != nullptr && attachmentImage->IsFromSwapChain()) {
                attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            } else {
                attachmentDescriptions[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
        }

        // depth stencil attachment
        if (config.depthStencilAttachment) {
            auto& depthStencilAttachmentDescription = attachmentDescriptions[attachmentDescriptions.size() - 1];
            depthStencilAttachmentDescription = basicAttachmentDescription;
            depthStencilAttachmentDescription.format = VkConvert<Format, VkFormat>(config.depthStencilAttachment->GetImage()->GetConfig().format);
            depthStencilAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        // attachment reference
        std::vector<VkAttachmentReference> colorAttachmentReferences(config.colorAttachments.size());
        for (uint32_t i = 0; i < colorAttachmentReferences.size() - 1; i++) {
            colorAttachmentReferences[i].attachment = i;
            colorAttachmentReferences[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        VkAttachmentReference depthStencilAttachmentReference {};
        if (config.depthStencilAttachment) {
            depthStencilAttachmentReference.attachment = colorAttachmentReferences.size();
            depthStencilAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        // sub pass
        VkSubpassDescription subPassDescription {};
        subPassDescription.flags = 0;
        subPassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPassDescription.inputAttachmentCount = 0;
        subPassDescription.pInputAttachments = nullptr;
        subPassDescription.colorAttachmentCount = colorAttachmentReferences.size();
        subPassDescription.pColorAttachments = colorAttachmentReferences.data();
        subPassDescription.pDepthStencilAttachment = config.depthStencilAttachment ? &depthStencilAttachmentReference : nullptr;
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

    void RenderPass::CreateFrameBuffer()
    {
        std::vector<VkImageView> attachments(config.colorAttachments.size());
        for (uint32_t i = 0; i < config.colorAttachments.size(); i++) {
            attachments[i] = config.colorAttachments[i]->GetVkImageView();
        }
        if (config.depthStencilAttachment) {
            attachments.emplace_back(config.depthStencilAttachment->GetVkImageView());
        }

        VkFramebufferCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = vkRenderPass;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = config.width;
        createInfo.height = config.height;
        createInfo.layers = config.layers;

        if (vkCreateFramebuffer(device.GetVkDevice(), &createInfo, nullptr, &vkFrameBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vulkan frame buffer");
        }
    }

    void RenderPass::DestroyFrameBuffer()
    {
        vkDestroyFramebuffer(device.GetVkDevice(), vkFrameBuffer, nullptr);
    }
}
