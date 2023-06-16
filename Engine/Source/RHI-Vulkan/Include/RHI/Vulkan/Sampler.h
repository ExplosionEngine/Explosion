//
// Created by Zach Lee on 2022/3/6.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <RHI/Sampler.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKSampler : public Sampler {
    public:
        NON_COPYABLE(VKSampler)
        VKSampler(VKDevice& device, const SamplerCreateInfo& createInfo);
        ~VKSampler() override;

        void Destroy() override;
        VkSampler GetVkSampler() const;

    private:
        void CreateSampler(const SamplerCreateInfo& createInfo);
        VKDevice& device;
        VkSampler vkSampler;
    };
}