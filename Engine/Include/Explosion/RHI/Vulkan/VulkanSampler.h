//
// Created by Administrator on 2021/5/29 0029.
//

#ifndef EXPLOSION_VULKANSAMPLER_H
#define EXPLOSION_VULKANSAMPLER_H

#include <vulkan/vulkan.h>

#include <Explosion/RHI/Common/Sampler.h>

namespace Explosion::RHI {
    class VulkanDriver;
    class VulkanDevice;

    class VulkanSampler : public Sampler {
    public:
        VulkanSampler(VulkanDriver& driver, Config config);
        ~VulkanSampler() override;
        const VkSampler& GetVkSampler();

    private:
        void CreateSampler();
        void DestroySampler();

        VulkanDriver& driver;
        VulkanDevice& device;
        VkSampler vkSampler;
    };
}

#endif //EXPLOSION_VULKANSAMPLER_H
