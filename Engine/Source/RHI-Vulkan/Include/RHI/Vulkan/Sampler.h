//
// Created by Zach Lee on 2022/3/6.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <RHI/Sampler.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanSampler : public Sampler {
    public:
        NonCopyable(VulkanSampler)
        VulkanSampler(VulkanDevice& inDevice, const SamplerCreateInfo& inCreateInfo);
        ~VulkanSampler() override;

        void Destroy() override;
        VkSampler GetNative() const;

    private:
        void CreateSampler(const SamplerCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkSampler nativeSampler;
    };
}