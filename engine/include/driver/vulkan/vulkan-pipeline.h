//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_VULKAN_PIPELINE_H
#define EXPLOSION_VULKAN_PIPELINE_H

#include <driver/pipeline.h>
#include <driver/vulkan/vulkan-context.h>

namespace Explosion {
    class VulkanPipeline : public Pipeline {
    public:
        VulkanPipeline(VulkanContext& tContext, const Pipeline::Descriptor& descriptor);
        ~VulkanPipeline() override;

    private:
        VulkanContext& context;
    };
}

#endif //EXPLOSION_VULKAN_PIPELINE_H
