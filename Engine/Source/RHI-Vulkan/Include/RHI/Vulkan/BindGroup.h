//
// Created by Zach Lee on 2022/3/20.
//


#pragma once

#include <vulkan/vulkan.h>

#include <RHI/BindGroup.h>

namespace RHI::Vulkan {
    class VulkanDevice;

    class VulkanBindGroup final : public BindGroup {
    public:
        NonCopyable(VulkanBindGroup)
        VulkanBindGroup(VulkanDevice& inDevice, const BindGroupCreateInfo& inCreateInfo);
        ~VulkanBindGroup() noexcept override;

        VkDescriptorSet GetNative() const;

    private:
        void CreateNativeDescriptorPool(const BindGroupCreateInfo& inCreateInfo);
        void CreateNativeDescriptorSet(const BindGroupCreateInfo& inCreateInfo);

        VulkanDevice& device;
        VkDescriptorSet nativeDescriptorSet;
        VkDescriptorPool nativeDescriptorPool;
    };
}