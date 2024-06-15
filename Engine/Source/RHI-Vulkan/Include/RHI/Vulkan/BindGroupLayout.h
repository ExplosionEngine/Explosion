//
// Created by Zach Lee on 2022/3/6.
//


#pragma once

#include <vulkan/vulkan.h>

#include <RHI/BindGroupLayout.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanBindGroupLayout final : public BindGroupLayout {
    public:
        NonCopyable(VulkanBindGroupLayout)
        VulkanBindGroupLayout(VulkanDevice& inDevice, const BindGroupLayoutCreateInfo& inCreateInfo);
        ~VulkanBindGroupLayout() override;

        VkDescriptorSetLayout GetNative() const;

    private:
        void CreateNativeDescriptorSetLayout(const BindGroupLayoutCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkDescriptorSetLayout nativeDescriptorSetLayout;
    };
}