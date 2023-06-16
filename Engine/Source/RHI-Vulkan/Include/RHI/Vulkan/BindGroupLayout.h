//
// Created by Zach Lee on 2022/3/6.
//


#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <RHI/BindGroupLayout.h>

namespace RHI::Vulkan {
    class VKDevice;

    class VKBindGroupLayout : public BindGroupLayout {
    public:
        NON_COPYABLE(VKBindGroupLayout)
        VKBindGroupLayout(VKDevice& device, const BindGroupLayoutCreateInfo& createInfo);
        ~VKBindGroupLayout() override;

        void Destroy() override;

        VkDescriptorSetLayout GetVkDescriptorSetLayout() const;

    private:
        void CreateDescriptorSetLayout(const BindGroupLayoutCreateInfo& createInfo);
        VKDevice& device;
        VkDescriptorSetLayout setLayout;
    };
}