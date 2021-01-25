//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_VULKAN_RENDER_PASS_H
#define EXPLOSION_VULKAN_RENDER_PASS_H

#include <driver/render-pass.h>
#include <driver/vulkan/vulkan-context.h>

namespace Explosion {
    class VulkanRenderPass : public RenderPass {
    public:
        VulkanRenderPass(VulkanContext& tContext, const RenderPass::Descriptor& tDescriptor);
        ~VulkanRenderPass() override;

    private:
        VulkanContext& context;
    };
}

#endif //EXPLOSION_VULKAN_RENDER_PASS_H
